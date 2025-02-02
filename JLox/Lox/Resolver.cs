namespace Lox;

public class Resolver(Interpreter interpreter) : Stmt.IVisitor<Void?>, Expr.IVisitor<Void?>
{
    private readonly Interpreter interpreter = interpreter;
    private readonly Stack<Dictionary<string, bool>> scopes = new Stack<Dictionary<string, bool>>();

    public Void? VisitBlockStmt(Stmt.Block stmt)
    {
        BeginScope();
        Resolve(stmt.Statements);
        EndScope();
        return null;
    }

    public Void? VisitExpressionStmt(Stmt.Expression stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitFunctionStmt(Stmt.Function stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitIfStmt(Stmt.If stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitPrintStmt(Stmt.Print stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitReturnStmt(Stmt.Return stmt)
    {
        throw new NotImplementedException();
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
        throw new NotImplementedException();
    }

    public Void? VisitBreakStmt(Stmt.Break stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitAssignExpr(Expr.Assign expr)
    {
        Resolve(expr.Value);
        ResolveLocal(expr, expr.Name);
        return null;
    }

    public Void? VisitBinaryExpr(Expr.Binary expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitGroupingExpr(Expr.Grouping expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitLiteralExpr(Expr.Literal expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitLogicalExpr(Expr.Logical expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitUnaryExpr(Expr.Unary expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitCallExpr(Expr.Call expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitVariableExpr(Expr.Variable expr)
    {
        if (scopes.Count > 0 && scopes.Peek()[expr.Name.Lexeme] == false)
        {
            Lox.Error(expr.Name, "Can't read local variable in its own initializer.");
        }

        ResolveLocal(expr, expr.Name);
        return null;
    }

    public Void? VisitFunctionExpr(Expr.Function expr)
    {
        throw new NotImplementedException();
    }

    private void Declare(Token name)
    {
        if (scopes.Count < 1) return;

        var scope = scopes.Peek();
        scope.Add(name.Lexeme, false);
    }

    private void Define(Token name)
    {
        if (scopes.Count < 1) return;
        scopes.Peek()[name.Lexeme] = true;
    }

    private void BeginScope() => scopes.Push(new Dictionary<string, bool>());

    private void EndScope() => scopes.Pop();

    private void Resolve(List<Stmt> statements)
    {
        foreach (var statement in statements)
        {
            Resolve(statement);
        }
    }

    private void Resolve(Stmt stmt) => stmt.Accept(this);

    private void Resolve(Expr expr) => expr.Accept(this);

    private void ResolveLocal(Expr expr, Token name)
    {
        for (var i = scopes.Count - 1; i > 0; i--)
        {
            if (scopes.ElementAt(i).ContainsKey(name.Lexeme))
            {
                interpreter.Resolve(expr, scopes.Count - 1 - i);
                return;
            }
        }
    }
}