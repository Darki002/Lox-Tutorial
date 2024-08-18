namespace Lox;

public class Token(TokenType tokenType, string lexeme, object literal, int line)
{
    public readonly TokenType TokenType = tokenType;
    public readonly string Lexeme = lexeme;
    public readonly object Literal = literal;
    public readonly int Line = line;


    public override string ToString()
    {
        return $"{TokenType} {Lexeme} {Literal}";
    }
}