using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using MsBox.Avalonia;
using MsBox.Avalonia.Base;
using MsBox.Avalonia.Enums;

using System;
using System.Diagnostics;
using System.IO;

using NManager.Models;

namespace NManager.Views;

public partial class MainWindow : Window
{
    private readonly MainModel model = new();

    public MainWindow()
    {
        InitializeComponent();
        DisplayContent(model.CurrentPath);

        pathTextField.Text = model.CurrentPath;
        backButton.IsEnabled = model.PreviousPaths.Count > 0;
        forwardButton.IsEnabled = model.NextPaths.Count > 0;
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
            model.FilesStartIndex = subdirectories.Length;

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

        pathTextField.Width = Width - totalToolbarButtonsLength;
        contentList.Height = Height - toolbar.Bounds.Height;
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

        if (contentList.SelectedIndex < model.FilesStartIndex)
        {
            string newCurrentPath = model.CurrentPath + slash + contentList.SelectedItem;

            if (DisplayContent(newCurrentPath))
            {
                model.PreviousPaths.Push(model.CurrentPath);

                if (model.NextPaths.Count != 0)
                {
                    model.NextPaths.Clear();
                }

                model.CurrentPath = newCurrentPath;
                pathTextField.Text = model.CurrentPath;
                backButton.IsEnabled = model.PreviousPaths.Count > 0;
                forwardButton.IsEnabled = model.NextPaths.Count > 0;
            }
        }
        else
        {
            Process process = new();
            ProcessStartInfo processStartInfo = new()
            {
                FileName = model.CurrentPath + slash + contentList.SelectedItem,
                UseShellExecute = true
            };
            process.StartInfo = processStartInfo;

            process.Start();
        }
    }

    private void BackButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        if (model.PreviousPaths.Count > 0)
        {
            model.NextPaths.Push(model.CurrentPath);

            model.CurrentPath = model.PreviousPaths.Pop();
            pathTextField.Text = model.CurrentPath;

            DisplayContent(model.CurrentPath);
        }

        backButton.IsEnabled = model.PreviousPaths.Count > 0;
        forwardButton.IsEnabled = model.NextPaths.Count > 0;
    }

    private void ForwardButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        if (model.NextPaths.Count > 0)
        {
            model.PreviousPaths.Push(model.CurrentPath);

            model.CurrentPath = model.NextPaths.Pop();
            pathTextField.Text = model.CurrentPath;

            DisplayContent(model.CurrentPath);
        }

        backButton.IsEnabled = model.PreviousPaths.Count > 0;
        forwardButton.IsEnabled = model.NextPaths.Count > 0;
    }

    private void ReloadButtonClickHandler(object? sender, RoutedEventArgs args)
    {
        DisplayContent(model.CurrentPath);
    }

    private void PathTextFieldKeyDownHandler(object? sender, KeyEventArgs args)
    {
        if (args.Key == Key.Enter && pathTextField.Text != null && pathTextField.Text.Length > 0 && DisplayContent(pathTextField.Text))
        {
            model.PreviousPaths.Push(model.CurrentPath);

            if (model.NextPaths.Count != 0)
            {
                model.NextPaths.Clear();
            }

            model.CurrentPath = pathTextField.Text;
        }

        backButton.IsEnabled = model.PreviousPaths.Count > 0;
        forwardButton.IsEnabled = model.NextPaths.Count > 0;
    }
}