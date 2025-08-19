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

    private void RemoveItemMenuSelectionChanged(object? sender, SelectionChangedEventArgs args)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.RemoveItemMenuIsEnabled = vm.ContentListSelectedItem != null;
        }
    }

    private void PathTextFieldKeyDown(object? sender, KeyEventArgs args)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.LoadPathFromPathTextField(args);
        }
    }

    private void ContentListDoubleTapped(object? sender, TappedEventArgs args)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.OpenFolderOrLaunchFile();
        }
    }

    private void RecalculateContentSize()
    {
        double totalToolbarButtonsLength = 0;

        for (int i = 0; i < toolbar.Children.Count; i++)
        {
            if (toolbar.Children[i] is not TextBox)
            {
                totalToolbarButtonsLength += toolbar.Children[i].Bounds.Width;
            }
        }

        pathTextField.Width = Width - totalToolbarButtonsLength;
        contentList.Height = Height - toolbar.Bounds.Height;
    }
}