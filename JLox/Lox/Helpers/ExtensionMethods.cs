namespace Lox.Helpers;

public static class ExtensionMethods
{
    public static string Sub(this string source, int startIndex, int endIndex)
    {
        return source.Substring(startIndex, endIndex - startIndex);
    }
}