namespace Lox.Errors;

public class Return(object? value) : Exception
{
    public readonly object? Value = value;
}