using Lox.Expressions;

namespace Lox;

public class Parser(List<Token> tokens)
{
    private int current = 0;

    public Expr? Parse()
    {
        try
        {
            return Expression();
        }
        catch (ParseError e)
        {
            return null;
        }
    }
    
    private Expr Expression()
    {
        return Equality();
    }

    private Expr Equality()
    {
        var expr = Comparison();

        while (Match(TokenType.BANG_EQUAL, TokenType.EQUAL_EQUAL))
        {
            var @operator = Previous();
            var right = Comparison();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private Expr Comparison()
    {
        var expr = Term();

        while (Match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL))
        {
            var @operator = Previous();
            var right = Term();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private Expr Term()
    {
        var expr = Factor();

        while (Match(TokenType.PLUS, TokenType.MINUS))
        {
            var @operator = Previous();
            var right = Factor();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private Expr Factor()
    {
        var expr = Unary();

        while (Match(TokenType.STAR, TokenType.SLASH))
        {
            var @operator = Previous();
            var right = Unary();
            expr = new Expr.Binary(expr, @operator, right);
        }

        return expr;
    }

    private Expr Unary()
    {
        if (Match(TokenType.BANG, TokenType.MINUS))
        {
            var @operator = Previous();
            var right = Unary();
            return new Expr.Unary(@operator, right);
        }
        return Primary();
    }

    private Expr Primary()
    {
        if (Match(TokenType.FALSE)) return new Expr.Literal(false);
        if (Match(TokenType.TRUE)) return new Expr.Literal(true);

        if (Match(TokenType.NIL)) return new Expr.Literal(null);

        if (Match(TokenType.NUMBER, TokenType.STRING))
        {
            return new Expr.Literal(Previous().Literal);
        }

        if (Match(TokenType.LEFT_PAREN))
        {
            var expr = Expression();
            Consume(TokenType.RIGHT_PAREN, "Expect ')' after expression.");
            return new Expr.Grouping(expr);
        }

        throw Error(Peek(), "Expect expression.");
    }
    
    private bool Match(params TokenType[] tokenTypes)
    {
        foreach (var type in tokenTypes)
        {
            if (Check(type))
            {
                Advance();
                return true;
            }
        }

        return false;
    }

    private bool Check(TokenType type)
    {
        if (IsAtEnd()) return false;
        return type == Peek().Type;
    }

    private Token Advance()
    {
        if (!IsAtEnd()) current++;
        return Previous();
    }

    private Token Previous()
    {
        return tokens[current - 1];
    }

    private Token Peek() => tokens[current];

    private Token Consume(TokenType type, string message)
    {
        if(Check(type)) return Advance();

        throw Error(Peek(), message);
    }

    private bool IsAtEnd() => Peek().Type == TokenType.EOF;

    private ParseError Error(Token token, string message)
    {
        Lox.Error(token, message);
        return new ParseError();
    }

    private void Synchronize()
    {
        Advance();

        while (!IsAtEnd())
        {
            if(Previous().Type == TokenType.SEMICOLON) return;

            switch (Peek().Type)
            {
                case TokenType.CLASS:
                case TokenType.FUN:
                case TokenType.VAR:
                case TokenType.FOR:
                case TokenType.IF:
                case TokenType.WHILE:
                case TokenType.PRINT:
                case TokenType.RETURN:
                    return;
            }

            Advance();
        }
    }
}