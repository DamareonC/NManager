using Avalonia.Input;
using CommunityToolkit.Mvvm.ComponentModel;
using MsBox.Avalonia;
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
    private bool _UpButtonIsEnabled;
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
        UpButtonIsEnabled = CurrentPathHasParent();

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

        UpdateButtons();
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

        UpdateButtons();
    }

    public void OnUpButtonClick()
    {
        bool hasParent = CurrentPathHasParent();
        
        if (hasParent)
        {
            string? newCurrentPath = Directory.GetParent(model.CurrentPath)?.FullName;

            if (newCurrentPath != null && LoadContent(newCurrentPath))
            {
                SetCurrentPath(newCurrentPath);
                UpdateButtons();

                PathTextFieldText = model.CurrentPath;
            }
        }
    }

    public void OnReloadButtonClick()
    {
        LoadContent(model.CurrentPath);
    }

    public void OpenFolderOrLaunchFile()
    {
        bool hasParent = CurrentPathHasParent();
        string slash = OperatingSystem.IsWindows() ? "\\" : "/";

        if (ContentListSelectedIndex < model.FilesStartIndex)
        {
            string newCurrentPath = model.CurrentPath + (hasParent ? slash : "") + ContentListSelectedItem;

            if (LoadContent(newCurrentPath))
            {
                SetCurrentPath(newCurrentPath);                
                UpdateButtons();

                PathTextFieldText = model.CurrentPath;
            }
        }
        else
        {
            Process process = new();
            ProcessStartInfo processStartInfo = new()
            {
                FileName = model.CurrentPath + (hasParent ? slash : "") + ContentListSelectedItem,
                UseShellExecute = true
            };
            process.StartInfo = processStartInfo;

            process.Start();
        }
    }

    public void LoadPathFromPathTextField(KeyEventArgs args)
    {
        if (args.Key == Key.Enter && PathTextFieldText != null && PathTextFieldText.Length > 0 && !PathTextFieldText.Equals(model.CurrentPath) && LoadContent(PathTextFieldText))
        {
            SetCurrentPath(PathTextFieldText);
        }

        UpdateButtons();
    }

    private void UpdateButtons()
    {
        BackButtonIsEnabled = model.PreviousPaths.Count > 0;
        ForwardButtonIsEnabled = model.NextPaths.Count > 0;
        UpButtonIsEnabled = CurrentPathHasParent();
    }

    private void SetCurrentPath(string newPath)
    {
        model.PreviousPaths.Push(model.CurrentPath);

        if (model.NextPaths.Count != 0)
        {
            model.NextPaths.Clear();
        }

        model.CurrentPath = newPath;
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
        catch (Exception e)
        {
            ShowErrorMessage(e);
            return false;
        }
    }

    private bool CurrentPathHasParent()
    {
        try
        {
            return Directory.GetParent(model.CurrentPath) != null;
        }
        catch (Exception e)
        {
            ShowErrorMessage(e);
            return false;
        }
    }

    private static void ShowErrorMessage(Exception e)
    {
        switch (e)
        {
            case ArgumentException:
                MessageBoxManager.GetMessageBoxStandard("Invalid Path", "The path contains only spaces or invalid charaters.", ButtonEnum.Ok, Icon.Error).ShowAsync();
                break;
            case DirectoryNotFoundException:
                MessageBoxManager.GetMessageBoxStandard("Could Not Find Folder", "The selected folder could not be found.", ButtonEnum.Ok, Icon.Error).ShowAsync();
                break;
            case UnauthorizedAccessException:
                MessageBoxManager.GetMessageBoxStandard("Permission Denied", "You do not have permission to open this folder.", ButtonEnum.Ok, Icon.Error).ShowAsync();
                break;
            default:
                MessageBoxManager.GetMessageBoxStandard("Error", "An error has occured.", ButtonEnum.Ok, Icon.Error).ShowAsync();
                break;
        }
    }
}