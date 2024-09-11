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

// ReSharper disable InconsistentNaming
public enum TokenType
{
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    EOF
}
// ReSharper restore InconsistentNaming
