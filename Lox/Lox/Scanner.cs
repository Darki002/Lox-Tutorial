namespace Lox;

public class Scanner(string source)
{
    private readonly string source = source;
    private readonly List<Token> tokens;
}