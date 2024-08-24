namespace Lox;

public class Token(TokenType tokenType, string lexeme, object? literal, int line)
{
    public readonly TokenType TokenType = tokenType;
    public readonly string Lexeme = lexeme; // The string representing this token, ex. "var"
    public readonly object? Literal = literal; // Parsed value so we can use the value in the correct type later on
    public readonly int Line = line;


    public override string ToString()
    {
        return $"{TokenType} {Lexeme} {Literal}";
    }
}
