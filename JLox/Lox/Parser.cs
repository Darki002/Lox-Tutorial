using Lox.Errors;

namespace Lox;

public class Parser(List<Token> tokens)
{
    private int current;

    private bool inLoop;

    public List<Stmt?> Parse()
    {
        var statements = new List<Stmt?>();
        
        while (!IsAtEnd())
        {
            statements.Add(Declaration());
        }

        return statements;
    }

    private Stmt? Declaration()
    {
        try
        {
            if (Match(TokenType.CLASS)) return ClassDeclaration();
            if (Match(TokenType.FUN)) return Function("function");
            if (Match(TokenType.VAR)) return VarDeclaration();
            return Statement();
        }
        catch
        {
            Synchronize();
            return null;
        }
    }
    
    private Stmt.Class ClassDeclaration()
    {
        var name = Consume(TokenType.IDENTIFIER, "Expect class name.");

        Expr.Variable? superclass = null;
        
        if (Match(TokenType.LESS))
        {
            Consume(TokenType.IDENTIFIER, "Expected superclass name.");
            superclass = new Expr.Variable(Previous());
        }
        
        Consume(TokenType.LEFT_BRACE, "Expect '{' before class body.");

        var methods = new List<Stmt.Function>();
        var classMethods = new List<Stmt.Function>();
        var operatorOverloads = new List<Stmt.Function>();

        while (!Check(TokenType.RIGHT_BRACE) && !IsAtEnd())
        {
            if (Match(TokenType.CLASS))
            {
                if (Match(TokenType.OPERATOR))
                {
                    var op = ConsumeAny("Expected operator that can be overloaded.", TokenType.PLUS, TokenType.MINUS,
                        TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL, 
                        TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL);
                    
                    var parameters = GetParameters("Method");
                    if (parameters == null || parameters.Count != 2) Error(op, "Operator overload must have 2 Parameters");
                    Consume(TokenType.LEFT_BRACE, "Expect '{' before Method body.");
                    var body = Block();
                    operatorOverloads.Add(new Stmt.Function(op, parameters, body));
                }
                else
                {
                    classMethods.Add(Function("Method"));
                }
            }
            else
            {
                methods.Add(Function("Method"));
            }
        }

        Consume(TokenType.RIGHT_BRACE, "Expect '}' after class body.");
        return new Stmt.Class(name, superclass, methods, classMethods, operatorOverloads);
    }
    
    private Stmt.Function Function(string kind)
    {
        var name = Consume(TokenType.IDENTIFIER, $"Expect {kind} name.");
        var parameters = GetParameters(kind);
        
        Consume(TokenType.LEFT_BRACE, $"Expect '{{' before {kind} body.");
        var body = Block();
        return new Stmt.Function(name, parameters, body);
    }

    private List<Token>? GetParameters(string kind)
    {
        List<Token>? parameters = null;
        if (kind != "Method" || Check(TokenType.LEFT_PAREN))
        {
            Consume(TokenType.LEFT_PAREN, $"Expect '(' after {kind} name."); 
            parameters = [];

            if (!Check(TokenType.RIGHT_PAREN))
            {
                do
                {
                    if (parameters.Count >= 255) {
                        Error(Peek(), "Can't have more than 255 parameters.");
                    }
                
                    parameters.Add(Consume(TokenType.IDENTIFIER, "Expect parameter name."));
                } while (Match(TokenType.COMMA));
            }
            
            Consume(TokenType.RIGHT_PAREN, "Expect ')' after parameters.");
        }

        return parameters;
    }
    
    private Stmt.Var VarDeclaration()
    {
        var name = Consume(TokenType.IDENTIFIER, "Expect variable name.");

        Expr? initializer = null;
        if (Match(TokenType.EQUAL))
        {
            initializer = Expression();
        }

        Consume(TokenType.SEMICOLON, "Expect ';' after variable declaration.");
        return new Stmt.Var(name, initializer);
    }

    private Stmt Statement()
    {
        if (Match(TokenType.FOR)) return ForStatement();
        if (Match(TokenType.IF)) return IfStatement();
        if (Match(TokenType.LEFT_BRACE)) return new Stmt.Block(Block());
        if (Match(TokenType.PRINT)) return PrintStatement();
        if (Match(TokenType.RETURN)) return ReturnStatement();
        if (Match(TokenType.WHILE)) return WhileStatement();
        if (Match(TokenType.BREAK)) return BreakStatement();
        return ExpressionStatement();
    }

    private Stmt ForStatement()
    {
        inLoop = true;
        Consume(TokenType.LEFT_PAREN, "Expect '(' after 'for'.");

        Stmt? initializer;
        if (Match(TokenType.SEMICOLON))
        {
            initializer = null;
        }
        else if (Match(TokenType.VAR))
        {
            initializer = VarDeclaration();
        }
        else
        {
            initializer = ExpressionStatement();
        }

        Expr? condition = null;
        if (!Check(TokenType.SEMICOLON))
        {
            condition = Expression();
        }

        Consume(TokenType.SEMICOLON, "Expect ';' after loop condition.");

        Expr? increment = null;
        if (!Check(TokenType.SEMICOLON))
        {
            increment = Expression();
        }
        Consume(TokenType.RIGHT_PAREN, "Expect ')' after for clauses.");

        var body = Statement();

        if (increment != null)
        {
            body = new Stmt.Block([body, new Stmt.Expression(increment)]);
        }

        condition ??= new Expr.Literal(true);
        body = new Stmt.While(condition, body);

        if (initializer != null)
        {
            body = new Stmt.Block([initializer, body]);
        }
        
        inLoop = false;
        return body;
    }

    private Stmt.If IfStatement()
    {
        Consume(TokenType.LEFT_PAREN, "Expect '(' after 'if'.");
        var expr = Expression();
        Consume(TokenType.RIGHT_PAREN, "Expect ')' after condition.");

        var thenBranch = Statement();
        Stmt? elseStatement = null;
        if (Match(TokenType.ELSE))
        {
            elseStatement = Statement();
        }

        return new Stmt.If(expr, thenBranch, elseStatement);
    }

    private List<Stmt> Block()
    {
        var statements = new List<Stmt>();
        
        while (!Check(TokenType.RIGHT_BRACE) && !IsAtEnd())
            statements.Add(Declaration()!); // Null suppression could lead to edge cases after Synchronize returns null in interpreter could hit null pointer

        Consume(TokenType.RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    private Stmt.Expression ExpressionStatement()
    {
        var expr = Expression();
        Consume(TokenType.SEMICOLON, "Expect ';' after value.");
        return new Stmt.Expression(expr);
    }

    private Stmt.Print PrintStatement()
    {
        var expr = Expression();
        Consume(TokenType.SEMICOLON, "Expect ';' after value.");
        return new Stmt.Print(expr);
    }
    
    private Stmt.Return ReturnStatement()
    {
        var keyword = Previous();
        var expr = Peek().Type == TokenType.SEMICOLON ? null : Expression();
        
        Consume(TokenType.SEMICOLON, "Expect ';' after return value.");
        return new Stmt.Return(keyword, expr);
    }
    
    private Stmt.While WhileStatement()
    {
        inLoop = true;
        Consume(TokenType.LEFT_PAREN, "Expect '(' after 'while'.");
        var condition = Expression();
        Consume(TokenType.RIGHT_PAREN, "Expect ')' after condition.");

        var body = Statement();

        inLoop = false;
        return new Stmt.While(condition, body);
    }
    
    private Stmt.Break BreakStatement()
    {
        var token = Previous();
        
        if (!inLoop)
        {
            Error(token, "A break statement is not allowed outside of a loop.");
        }
        
        Consume(TokenType.SEMICOLON, "Expect ';' after 'break'.");
        return new Stmt.Break(token);
    }

    private Expr Expression()
    {
        return Assignment();
    }

    private Expr Assignment()
    {
        var expr = LogicalOr();

        if (Match(TokenType.EQUAL))
        {
            var equals = Previous();
            var value = Assignment();

            if (expr is Expr.Variable variable)
            {
                var name = variable.Name;
                return new Expr.Assign(name, value);
            }

            if (expr is Expr.Get get)
            {
                return new Expr.Set(get.Obj, get.Name, value);
            }

            if (expr is Expr.IndexGet indexGet)
            {
                return new Expr.IndexSet(indexGet.Obj, indexGet.Index, value, indexGet.Token);
            }

            Error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    private Expr LogicalOr()
    {
        var left = LogicalAnd();

        while (Match(TokenType.OR))
        {
            var opr = Previous();
            var right = LogicalAnd();
            left = new Expr.Logical(left, opr, right);
        }

        return left;
    }

    private Expr LogicalAnd()
    {
        var left = Equality();
        
        while (Match(TokenType.AND))
        {
            var opr = Previous();
            var right = Equality();
            left = new Expr.Logical(left, opr, right);
        }

        return left;
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

        while (Match(TokenType.STAR, TokenType.SLASH, TokenType.PERCENT))
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
        return Call();
    }

    private Expr Call()
    {
        var expr = Primary();

        while (true)
        {
            if (Match(TokenType.LEFT_PAREN))
            {
                expr = FinishCall(expr);
            }
            else if (Match(TokenType.DOT))
            {
                var name = Consume(TokenType.IDENTIFIER, "Expect property name after '.'.");
                expr = new Expr.Get(expr, name);
            }
            else if (Match(TokenType.LEFT_SQUARE_BRACKET))
            {
                var token = Previous();
                var index = Expression();
                expr = new Expr.IndexGet(expr, index, token);
                Consume(TokenType.RIGHT_SQUARE_BRACKET, "Expected closing ']'.");
            }
            else
            {
                break;
            }
        }

        return expr;
    }

    private Expr.Call FinishCall(Expr expr)
    {
        var arguments = new List<Expr>();

        if (!Check(TokenType.RIGHT_PAREN))
        {
            do
            {
                if (arguments.Count > 255)
                {
                    Error(Peek(), "Can't have more than 255 arguments.");
                }
                arguments.Add(Expression());
            } while (Match(TokenType.COMMA));
        }

        var paren = Consume(TokenType.RIGHT_PAREN, "Expect ')' after arguments.");
        return new Expr.Call(expr, paren, arguments);
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

        if (Match(TokenType.SUPER))
        {
            var keyword = Previous();
            Consume(TokenType.DOT, "Expect '.' after 'super'.");
            var method = Consume(TokenType.IDENTIFIER, "Expect superclass method name.");
            return new Expr.Super(keyword, method);
        }

        if (Match(TokenType.THIS)) return new Expr.This(Previous());

        if (Match(TokenType.IDENTIFIER)) return new Expr.Variable(Previous());

        if (Match(TokenType.LEFT_PAREN))
        {
            var expr = Expression();
            Consume(TokenType.RIGHT_PAREN, "Expect ')' after expression.");
            return new Expr.Grouping(expr);
        }

        if (Match(TokenType.FUN))
        {
            Consume(TokenType.LEFT_PAREN, "Expect '(' after 'fun' for anonymous functions."); 
        
            List<Token> parameters = [];
            if (!Check(TokenType.RIGHT_PAREN))
            {
                do
                {
                    if (parameters.Count >= 255) {
                        Error(Peek(), "Can't have more than 255 parameters.");
                    }
                    
                    parameters.Add(Consume(TokenType.IDENTIFIER, "Expect parameter name."));
                } while (Match(TokenType.COMMA));
            }
            Consume(TokenType.RIGHT_PAREN, "Expect ')' after parameters.");
            Consume(TokenType.LEFT_BRACE, "Expect '{' before anonymous functions body.");
            
            var body = Block();
            return new Expr.Function(parameters, body);
        }

        if (Match(TokenType.LEFT_SQUARE_BRACKET))
        {
            var arguments = new List<Expr>();
            
            if (!Check(TokenType.RIGHT_SQUARE_BRACKET))
            {
                do
                {
                    arguments.Add(Expression());
                } while (Match(TokenType.COMMA));
            }
            
            Consume(TokenType.RIGHT_SQUARE_BRACKET, "Expected ']' after arguments.");
            return new Expr.Array(arguments);
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
    
    private Token ConsumeAny(string message, params TokenType[] type)
    {
        if(Match(type)) return Previous();

        throw Error(Peek(), message);
    }

    private bool IsAtEnd() => Peek().Type == TokenType.EOF;

    private static ParseError Error(Token token, string message)
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