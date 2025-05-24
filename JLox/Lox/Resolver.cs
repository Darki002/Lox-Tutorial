namespace Lox;

public class Resolver(Interpreter interpreter) : Stmt.IVisitor<Void?>, Expr.IVisitor<Void?>
{
    private readonly Stack<Dictionary<string, int>> scopes = new();
    
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

    public Void? VisitClassStmt(Stmt.Class stmt)
    {
        Declare(stmt.Name);
        
        if(stmt.Superclass is not null) Resolve(stmt.Superclass);
        
        BeginScope();
        scopes.Peek()["this"] = 0;
        foreach (var method in stmt.ClassMethods)
        {
            ResolveFunction(method);
        }
        EndScope();
        
        BeginScope();
        scopes.Peek()["this"] = 0;
        foreach (var method in stmt.Methods)
        {
            ResolveFunction(method);
        }
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
        ResolveFunction(stmt);
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

        return null;
    }

    public Void? VisitWhileStmt(Stmt.While stmt)
    {
        Resolve(stmt.Condition);
        Resolve(stmt.Body);
        return null;
    }

    public Void? VisitBreakStmt(Stmt.Break stmt) => null;

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

    public Void? VisitSetExpr(Expr.Set expr)
    {
        Resolve(expr.Value);
        Resolve(expr.Obj);
        return null;
    }

    public Void? VisitThisExpr(Expr.This expr)
    {
        ResolveLocal(expr, expr.Keyword);
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

    public Void? VisitGetExpr(Expr.Get expr)
    {
        Resolve(expr.Obj);
        return null;
    }

    public Void? VisitVariableExpr(Expr.Variable expr)
    {
        ResolveLocal(expr, expr.Name);
        return null;
    }

    public Void? VisitFunctionExpr(Expr.Function expr)
    {
        BeginScope();
        foreach (var param in expr.Params)
        {
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
        scope.Add(name.Lexeme, scope.Count);
    }

    private void BeginScope() => scopes.Push(new());
    private void EndScope() => scopes.Pop();
    
    private void Resolve(Stmt stmt) => stmt.Accept(this);
    private void Resolve(List<Stmt> statements) => statements.ForEach(Resolve);

    private void Resolve(Expr expr) => expr.Accept(this);
    private void Resolve(List<Expr> expr) => expr.ForEach(Resolve);

    private void ResolveLocal(Expr expr, Token name)
    {
        for (var i = 0; i < scopes.Count; i++)
        {
            var scope = scopes.ElementAt(i);
            if (scope.TryGetValue(name.Lexeme, out var index))
            {
                interpreter.Resolve(expr, i, index);
                return;
            }
        }
    }

    private void ResolveFunction(Stmt.Function function)
    {
        BeginScope();

        if (function.Params is not null)
        {
            foreach (var param in function.Params)
            {
                Declare(param);
            }
        }
        
        Resolve(function.Body);
        EndScope();
    }
}