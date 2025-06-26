using Avalonia.Controls;
using Avalonia.Input;
using MsBox.Avalonia;
using MsBox.Avalonia.Base;
using MsBox.Avalonia.Enums;

using System;
using System.Diagnostics;
using System.IO;

namespace NManager;

public partial class MainWindow : Window
{
    private string currentPath = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
    private int filesStartIndex = 0;

    public MainWindow()
    {
        InitializeComponent();
        DisplayContent(currentPath);
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

    private void ContentList_DoubleTappedHandler(object? sender, TappedEventArgs args)
    {
        if (contentList.SelectedIndex < filesStartIndex)
        {
            string newCurrentPath = currentPath + "/" + contentList.SelectedItem;

            if (DisplayContent(newCurrentPath))
            {
                currentPath = newCurrentPath;
            }
        }
        else
        {
            Process process = new();
            ProcessStartInfo processStartInfo = new()
            {
                FileName = currentPath + "/" + contentList.SelectedItem,
                UseShellExecute = true
            };
            process.StartInfo = processStartInfo;

            process.Start();
        }
    }
}