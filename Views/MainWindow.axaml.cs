using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using MsBox.Avalonia;
using MsBox.Avalonia.Base;
using MsBox.Avalonia.Enums;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace NManager;

public partial class MainWindow : Window
{
    private readonly Stack<string> previousPaths = new(), nextPaths = new();
    private string currentPath = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
    private int filesStartIndex = 0;

    public MainWindow()
    {
        InitializeComponent();
        DisplayContent(currentPath);

        pathTextField.Text = currentPath;
        backButton.IsEnabled = previousPaths.Count > 0;
        forwardButton.IsEnabled = nextPaths.Count > 0;
    }

    private bool DisplayContent(string currentPath)
    {
        try
        {
            DirectoryInfo directory = new(currentPath);
            DirectoryInfo[] subdirectories = directory.GetDirectories();
            FileInfo[] files = directory.GetFiles();
            string[] content = new string[subdirectories.Length + files.Length];

            for (int i = 0; i < subdirectories.Length; i++)
            {
                content[i] = subdirectories[i].Name;
            }

            for (int i = subdirectories.Length; i < subdirectories.Length + files.Length; i++)
            {
                content[i] = files[i - subdirectories.Length].Name;
            }

            contentList.ItemsSource = content;
            filesStartIndex = subdirectories.Length;

            return true;
        }
        catch (DirectoryNotFoundException)
        {
            IMsBox<ButtonResult> messageBox = MessageBoxManager.GetMessageBoxStandard("Could Not Find Folder", "The selected folder could not be found.", ButtonEnum.Ok, MsBox.Avalonia.Enums.Icon.Error);
            messageBox.ShowAsync();

            return false;
        }
        catch (UnauthorizedAccessException)
        {
            IMsBox<ButtonResult> messageBox = MessageBoxManager.GetMessageBoxStandard("Permission Denied", "You do not have permission to open this folder.", ButtonEnum.Ok, MsBox.Avalonia.Enums.Icon.Error);
            messageBox.ShowAsync();

            return false;
        }
    }

    private void RecalculateContentSize()
    {
        double totalToolbarButtonsLength = 0;

        for (int i = 0; i < toolbar.Children.Count - 1; i++)
        {
            totalToolbarButtonsLength += toolbar.Children[i].Bounds.Width;
        }

        pathTextField.Width = this.Width - totalToolbarButtonsLength;
        contentList.Height = this.Height - toolbar.Bounds.Height;
    }

    private void MainWindowLoadedHandler(object? sender, RoutedEventArgs args)
    {
        RecalculateContentSize();
    }

    private void MainWindowResizedHandler(object? sender, WindowResizedEventArgs args)
    {
        RecalculateContentSize();
    }

    private void ContentListDoubleTappedHandler(object? sender, TappedEventArgs args)
    {
        string slash = OperatingSystem.IsWindows() ? "\\" : "/";

        if (contentList.SelectedIndex < filesStartIndex)
        {
            string newCurrentPath = currentPath + slash + contentList.SelectedItem;

            if (DisplayContent(newCurrentPath))
            {
                previousPaths.Push(currentPath);

                if (nextPaths.Count != 0)
                {
                    nextPaths.Clear();
                }

                currentPath = newCurrentPath;
                pathTextField.Text = currentPath;
                backButton.IsEnabled = previousPaths.Count > 0;
                forwardButton.IsEnabled = nextPaths.Count > 0;
            }
        }
        else
        {
            Process process = new();
            ProcessStartInfo processStartInfo = new()
            {
                FileName = currentPath + slash + contentList.SelectedItem,
                UseShellExecute = true
            };
            process.StartInfo = processStartInfo;

            process.Start();
        }
    }

    private void BackButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        if (previousPaths.Count > 0)
        {
            nextPaths.Push(currentPath);

            currentPath = previousPaths.Pop();
            pathTextField.Text = currentPath;

            DisplayContent(currentPath);
        }

        backButton.IsEnabled = previousPaths.Count > 0;
        forwardButton.IsEnabled = nextPaths.Count > 0;
    }

    private void ForwardButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        if (nextPaths.Count > 0)
        {
            previousPaths.Push(currentPath);

            currentPath = nextPaths.Pop();
            pathTextField.Text = currentPath;

            DisplayContent(currentPath);
        }

        backButton.IsEnabled = previousPaths.Count > 0;
        forwardButton.IsEnabled = nextPaths.Count > 0;
    }

    private void ReloadButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        DisplayContent(currentPath);
    }

    private void PathTextFieldKeyDownHandler(object? sender, KeyEventArgs args)
    {
        if (args.Key == Key.Enter && pathTextField.Text != null && pathTextField.Text.Length > 0 && DisplayContent(pathTextField.Text))
        {
            previousPaths.Push(currentPath);

            if (nextPaths.Count != 0)
            {
                nextPaths.Clear();
            }

            currentPath = pathTextField.Text;
        }

        backButton.IsEnabled = previousPaths.Count > 0;
        forwardButton.IsEnabled = nextPaths.Count > 0;
    }
}