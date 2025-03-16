using Lox.Tree;

namespace Lox;

public class Resolver(Interpreter interpreter) : Stmt.IVisitor<Void?>, Expr.IVisitor<Void?>
{
    private readonly Stack<Dictionary<string, Variable>> scopes = new();
    private FunctionType currentFunction = FunctionType.NONE;

    public void Start(List<Stmt?> statements)
    {
        foreach (var stmt in statements)
        {
            if(stmt is not null) Resolve(stmt);
        }
    }
    
    public Void? VisitBlockStmt(Stmt.Block stmt)
    {
        BeginScope();
        Resolve(stmt.Statements);
        EndScope();
        return null;
    }

    public Void? VisitExpressionStmt(Stmt.Expression stmt)
    {
        Resolve(stmt.Body);
        return null;
    }

    public Void? VisitFunctionStmt(Stmt.Function stmt)
    {
        Declare(stmt.Name);
        Define(stmt.Name);

        ResolveFunction(stmt, FunctionType.FUNCTION);
        return null;
    }

    public Void? VisitIfStmt(Stmt.If stmt)
    {
        Resolve(stmt.Condition);
        Resolve(stmt.ThenBranch);
        if(stmt.ElseBranch is not null) Resolve(stmt.ElseBranch);
        return null;
    }

    public Void? VisitPrintStmt(Stmt.Print stmt)
    {
        Resolve(stmt.Right);
        return null;
    }

    public Void? VisitReturnStmt(Stmt.Return stmt)
    {
        if (currentFunction == FunctionType.NONE)
        {
            Lox.Error(stmt.Keyword, "Can't return from top-level code.");
        }
        
        if(stmt.Value is not null) Resolve(stmt.Value);
        return null;
    }

    public Void? VisitVarStmt(Stmt.Var stmt)
    {
        Declare(stmt.Name);
        if (stmt.Initializer != null)
        {
            Resolve(stmt.Initializer);
        }

        Define(stmt.Name);
        return null;
    }

    public Void? VisitWhileStmt(Stmt.While stmt)
    {
        var enclosing = currentFunction;
        currentFunction = FunctionType.WHILE;
        
        Resolve(stmt.Condition);
        Resolve(stmt.Body);
        
        currentFunction = enclosing;
        return null;
    }

    public Void? VisitBreakStmt(Stmt.Break stmt)
    {
        if (currentFunction != FunctionType.WHILE)
        {
            Lox.Error(stmt.keyword, "Can't break when not inside of a loop.");
        }
        return null;
    }

    public Void? VisitAssignExpr(Expr.Assign expr)
    {
        Resolve(expr.Value);
        ResolveLocal(expr, expr.Name);
        return null;
    }

    public Void? VisitBinaryExpr(Expr.Binary expr)
    {
        Resolve(expr.Right);
        Resolve(expr.Left);
        return null;
    }

    public Void? VisitGroupingExpr(Expr.Grouping expr)
    {
        Resolve(expr.Expression);
        return null;
    }

    public Void? VisitLiteralExpr(Expr.Literal expr) => null;

    public Void? VisitLogicalExpr(Expr.Logical expr)
    {
        Resolve(expr.Left);
        Resolve(expr.Right);
        return null;
    }

    public Void? VisitUnaryExpr(Expr.Unary expr)
    {
        Resolve(expr.Right);
        return null;
    }

    public Void? VisitCallExpr(Expr.Call expr)
    {
        Resolve(expr.Callee);
        Resolve(expr.Arguments);
        return null;
    }

    public Void? VisitVariableExpr(Expr.Variable expr)
    {
        if (scopes.Count > 0 && scopes.Peek().GetValueOrDefault(expr.Name.Lexeme)?.IsDefine == false)
        {
            Lox.Error(expr.Name, "Can't read local variable in its own initializer.");
        }

        ResolveLocal(expr, expr.Name);
        return null;
    }

    public Void? VisitFunctionExpr(Expr.Function expr)
    {
        BeginScope();
        foreach (var param in expr.Params)
        {
            Define(param);
            Declare(param);
        }
        
        Resolve(expr.Body);
        EndScope();
        return null;
    }

    private void Declare(Token name)
    {
        if (scopes.Count < 1) return;

        var scope = scopes.Peek();

        if (scope.ContainsKey(name.Lexeme))
        {
            Lox.Error(name, "Already a variable with this name in this scope.");
        }
        
        scope.Add(name.Lexeme, new Variable(name));
    }

    private void Define(Token name)
    {
        if (scopes.Count < 1) return;
        
        var variable = scopes.Peek()[name.Lexeme];
        variable.IsDefine = true;
        variable.Index = scopes.Peek().Count - 1;
    }

    private void BeginScope() => scopes.Push(new());

    private void EndScope()
    {
        var scope = scopes.Pop();
        foreach (var variable in scope)
        {
            if (variable.Value.IsUsed == false)
            {
                Lox.Warn(variable.Value.Token, "Variable is not being used inside of it's scope.");
            }
        }
    }
    
    private void Resolve(Stmt stmt) => stmt.Accept(this);
    private void Resolve(List<Stmt> statements) => statements.ForEach(Resolve);

    private void Resolve(Expr expr) => expr.Accept(this);
    private void Resolve(List<Expr> expr) => expr.ForEach(Resolve);

    private void ResolveLocal(Expr expr, Token name)
    {
        for (var i = scopes.Count - 1; i >= 0; i--)
        {
            if (scopes.ElementAt(i).TryGetValue(name.Lexeme, out var value))
            {
                value.IsUsed = true;
                interpreter.Resolve(expr, scopes.Count - 1 - i, value.Index);
                return;
            }
        }
    }

    private void ResolveFunction(Stmt.Function function, FunctionType type)
    {
        var enclosingFunction = currentFunction;
        currentFunction = type;
        
        BeginScope();
        foreach (var param in function.Params)
        {
            Declare(param);
            Define(param);
        }
        
        Resolve(function.Body);
        EndScope();
        currentFunction = enclosingFunction;
    }
    
    private enum FunctionType
    {
        NONE,
        FUNCTION,
        WHILE
    }
    
    private class Variable(Token token)
    {
        public bool IsDefine { get; set; } = false;

        public bool IsUsed { get; set; } = false;

        public Token Token { get; } = token;

        public int Index { get; set; } = -1;
    }
}