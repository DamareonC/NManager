using Avalonia.Input;
using CommunityToolkit.Mvvm.ComponentModel;
using MsBox.Avalonia;
using MsBox.Avalonia.Base;
using MsBox.Avalonia.Enums;

using System;
using System.Diagnostics;
using System.IO;

using NManager.Models;

namespace NManager.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    private readonly MainModel model = new();
    [ObservableProperty]
    private bool _BackButtonIsEnabled;
    [ObservableProperty]
    private bool _ForwardButtonIsEnabled;
    [ObservableProperty]
    private int _ContentListSelectedIndex;
    [ObservableProperty]
    private object? _ContentListSelectedItem;
    [ObservableProperty]
    private string? _PathTextFieldText;
    [ObservableProperty]
    private string[]? _ContentListItemsSource;

    public MainWindowViewModel()
    {
        PathTextFieldText = model.CurrentPath;
        
        LoadContent(model.CurrentPath);
    }

    public void OnBackButtonClick()
    {
        if (model.PreviousPaths.Count > 0)
        {
            model.NextPaths.Push(model.CurrentPath);

            model.CurrentPath = model.PreviousPaths.Pop();
            PathTextFieldText = model.CurrentPath;

            LoadContent(model.CurrentPath);
        }

        BackButtonIsEnabled = model.PreviousPaths.Count > 0;
        ForwardButtonIsEnabled = model.NextPaths.Count > 0;
    }

    public void OnForwardButtonClick()
    {
        if (model.NextPaths.Count > 0)
        {
            model.PreviousPaths.Push(model.CurrentPath);

            model.CurrentPath = model.NextPaths.Pop();
            PathTextFieldText = model.CurrentPath;

            LoadContent(model.CurrentPath);
        }

        BackButtonIsEnabled = model.PreviousPaths.Count > 0;
        ForwardButtonIsEnabled = model.NextPaths.Count > 0;
    }

    public void OnReloadButtonClick()
    {
        LoadContent(model.CurrentPath);
    }

    public void OpenFolderOrLaunchFile()
    {
        string slash = OperatingSystem.IsWindows() ? "\\" : "/";

        if (ContentListSelectedIndex < model.FilesStartIndex)
        {
            bool isRoot = Directory.GetParent(model.CurrentPath) == null;
            string newCurrentPath = model.CurrentPath + (isRoot ? "" : slash) + ContentListSelectedItem;

            if (LoadContent(newCurrentPath))
            {
                model.PreviousPaths.Push(model.CurrentPath);

                if (model.NextPaths.Count != 0)
                {
                    model.NextPaths.Clear();
                }

                model.CurrentPath = newCurrentPath;
                PathTextFieldText = model.CurrentPath;
                BackButtonIsEnabled = model.PreviousPaths.Count > 0;
                ForwardButtonIsEnabled = model.NextPaths.Count > 0;
            }
        }
        else
        {
            Process process = new();
            ProcessStartInfo processStartInfo = new()
            {
                FileName = model.CurrentPath + slash + ContentListSelectedItem,
                UseShellExecute = true
            };
            process.StartInfo = processStartInfo;

            process.Start();
        }
    }

    public void LoadPathFromPathTextField(KeyEventArgs args)
    {
        if (args.Key == Key.Enter && PathTextFieldText != null && PathTextFieldText.Length > 0 && LoadContent(PathTextFieldText))
        {
            model.PreviousPaths.Push(model.CurrentPath);

            if (model.NextPaths.Count != 0)
            {
                model.NextPaths.Clear();
            }

            model.CurrentPath = PathTextFieldText;
        }

        BackButtonIsEnabled = model.PreviousPaths.Count > 0;
        ForwardButtonIsEnabled = model.NextPaths.Count > 0;
    }

    private bool LoadContent(string currentPath)
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

            ContentListItemsSource = content;
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
}