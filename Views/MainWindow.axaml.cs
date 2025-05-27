using Avalonia.Controls;
using System;
using System.IO;

namespace NManager;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        string currentDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);

        DirectoryInfo directory = new(currentDirectory);
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
    }
}