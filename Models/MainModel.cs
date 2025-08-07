using System;
using System.Collections.Generic;

namespace NManager.Models;

public class MainModel
{
    public Stack<string> PreviousPaths { get; } = new();
    public Stack<string> NextPaths { get; } = new();
    public string CurrentPath { set; get; } = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
    public int FilesStartIndex { set; get; } = 0;
}