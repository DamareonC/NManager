using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;

using NManager.ViewModels;

namespace NManager.Views;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void MainWindowLoaded(object? sender, RoutedEventArgs args)
    {
        RecalculateContentSize();
    }

    private void MainWindowResized(object? sender, WindowResizedEventArgs args)
    {
        RecalculateContentSize();
    }

    private void ContentListDoubleTapped(object? sender, TappedEventArgs args)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.OpenFolderOrLaunchFile();
        }
    }

    private void PathTextFieldKeyDown(object? sender, KeyEventArgs args)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.LoadPathFromPathTextField(args);
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
}