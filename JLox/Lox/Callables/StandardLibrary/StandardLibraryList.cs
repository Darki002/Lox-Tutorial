namespace Lox.Callables.StandardLibrary;

public static class StandardLibraryList
{
    public static readonly Dictionary<string, object?> StandardLibFunctions =
        new()
        {
            {"clock", new Clock()}
        };
}