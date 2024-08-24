namespace Lox;

public class Scanner(string source)
{
    private readonly string source = source;
    private readonly List<Token> tokens;

    private int start = 0;
    private int current = 0;
    private int line = 1;

    public List<Token> ScanTokens()
    {
        while (!IsAtEnd())
        {
            start = current;
            ScanToken();
        }
        
        tokens.Add(new Token(TokenType.EOF, "", null, line));
        return tokens;
    }

    private void ScanToken()
    {
        var c = Advance();

        switch (c)
        {
            case '(':
                AddToken(TokenType.LEFT_PAREN);
                break;
            case ')':
                AddToken(TokenType.RIGHT_PAREN);
                break;
            case '{':
                AddToken(TokenType.LEFT_BRACE);
                break;
            case '}':
                AddToken(TokenType.RIGHT_BRACE);
                break;
            case ',':
                AddToken(TokenType.COMMA);
                break;
            case '.':
                AddToken(TokenType.DOT);
                break;
            case '-':
                AddToken(TokenType.MINUS);
                break;
            case '+':
                AddToken(TokenType.PLUS);
                break;
            case ';':
                AddToken(TokenType.SEMICOLON);
                break;
            case '*':
                AddToken(TokenType.STAR);
                break;
            case '!':
                AddToken(Match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
                break;
            case '=':
                AddToken(Match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
                break;
            case '>':
                AddToken(Match('=') ? TokenType.GREATER_EQUAL : TokenType.GREATER);
                break;
            case '<':
                AddToken(Match('=') ? TokenType.LESS_EQUAL : TokenType.LESS);
                break;
            case '"':
                ConsumeString();
                break;
            case '/':
                if (Match('/'))
                    SkipLine();
                else 
                    AddToken(TokenType.SLASH);
                break;
            case ' ' or '\r' or '\t':
                break;
            case '\n':
                line++;
                break;
            default:
                if (IsDigit(c))
                {
                    ConsumeNumber();
                    break;
                }
                
                Lox.Error(line, "Unexpected character.");
                break;
        }
    }

    private void AddToken(TokenType tokenType, object? literal = null)
    {
        var text = source.Substring(start, current - start);
        tokens.Add(new Token(tokenType, text, literal, line));
    }
    
    private char Advance()
    {
        return source[current++];
    }
    
    private char Peek()
    {
        return IsAtEnd() ? '\0' : source[current];
    }
    private char PeekNext()
    {
        return current + 1 >= source.Length ? '\0' : source[current + 1];
    }

    private bool Match(char expected)
    {
        if (IsAtEnd()) return false;
        if (source[current] != expected) return false;

        current++;
        return true;
    }

    private void ConsumeString()
    {
        while (Peek() != '"' && !IsAtEnd())
        {
            if (Peek() == '\n') line++;
            Advance();
        }

        if (IsAtEnd())
        {
            Lox.Error(line, "Unterminated string.");
            return;
        }

        Advance();

        // ReSharper disable LocalVariableHidesMember
        var start = this.start + 1;
        var current = this.current - 1;
        // ReSharper restore LocalVariableHidesMember
        
        var value = source.Substring(start, current - start);
        AddToken(TokenType.STRING, value);
    }

    private void ConsumeNumber()
    {
        while (IsDigit(Peek())) Advance();

        if (Peek() == '.' && IsDigit(PeekNext()))
        {
            Advance();
            while (IsDigit(Peek())) Advance();
        }

        if (IsAtEnd())
        {
            Lox.Error(line, "Missing Semicolon.");
            return;
        }
        
        var value = source.Substring(start, current - start);
        AddToken(TokenType.NUMBER, double.Parse(value));
    }
    
    private void SkipLine()
    {
        while (Peek() != '\n' && !IsAtEnd()) Advance();
    }
    
    private bool IsDigit(char c) {
        return c >= '0' && c <= '9';
    }

    private bool IsAtEnd()
    {
        return current >= source.Length;
    }
}