namespace Lox;

/// <param name="lexeme">The string representing this token, ex. "var"</param>
/// <param name="literal">Parsed value so we can use the value in the correct type later on</param>
public class Token(TokenType type, string lexeme, object? literal, int line)
{
    public readonly TokenType Type = type;
    
    /// <summary>
    /// The string representing this token, ex. "var"
    /// </summary>
    public readonly string Lexeme = lexeme;
    
    /// <summary>
    /// Parsed value so we can use the value in the correct type later on
    /// </summary>
    public readonly object? Literal = literal;
    
    public readonly int Line = line;


    public override string ToString()
    {
        return $"{Type} {Lexeme} {Literal}";
    }
}
