using Lox.Callables.StandardLibrary;

namespace Lox;

public class Analyzer: Stmt.IVisitor<Void?>, Expr.IVisitor<Void?>
{
    private readonly Stack<Dictionary<string, Variable>> scopes = new();
    
    private FunctionType currentFunction = FunctionType.NONE;
    private bool isLoop = false;
    
    public void Start(List<Stmt?> statements)
    {
        BeginScope();
        foreach (var stmt in statements)
        {
            if(stmt is not null) Resolve(stmt);
        }
        EndScope();
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

        foreach (var method in stmt.Methods)
        {
            ResolveFunction(method, FunctionType.METHOD);
        }
        
        Define(stmt.Name);
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
        if(stmt.Initializer is not null) Resolve(stmt.Initializer);
        Define(stmt.Name);
        return null;
    }

    public Void? VisitWhileStmt(Stmt.While stmt)
    {
        var enclosing = isLoop;
        isLoop = true;
        
        Resolve(stmt.Condition);
        Resolve(stmt.Body);

        isLoop = enclosing;
        return null;
    }

    public Void? VisitBreakStmt(Stmt.Break stmt)
    {
        if (!isLoop)
        {
            Lox.Error(stmt.Keyword, "Can't break when not inside of a loop.");
        }
        return null;
    }

    public Void? VisitAssignExpr(Expr.Assign expr)
    {
        Resolve(expr.Value);
        ResolveLocal(expr.Name);
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

    public Void? VisitThisExpr(Expr.This expr) => null;

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
        if (scopes.Peek().GetValueOrDefault(expr.Name.Lexeme)?.VarState == VarState.DECLARED)
        {
            Lox.Error(expr.Name, "Can't read local variable in its own initializer.");
        }

        ResolveLocal(expr.Name);
        return null;
    }

    public Void? VisitFunctionExpr(Expr.Function expr)
    {
        BeginScope();
        foreach (var param in expr.Params)
        {
            Declare(param);
            Define(param);
        }
        
        Resolve(expr.Body);
        EndScope();
        return null;
    }
    
    private void Declare(Token name)
    {
        var scope = scopes.Peek();

        if (scope.ContainsKey(name.Lexeme))
        {
            Lox.Error(name, "Already a variable with this name in this scope.");
        }

        if (StandardLibraryList.StandardLibFunctions.ContainsKey(name.Lexeme))
        {
            Lox.Warn(name, $"Shadows build-in variable ${name.Lexeme}.");
        }
        
        scope.Add(name.Lexeme, new Variable(name));
    }

    private void Define(Token name)
    {
        scopes.Peek()[name.Lexeme].VarState = VarState.DEFINED;
    }
    
    private void BeginScope() => scopes.Push(new());
    
    private void EndScope()
    {
        var scope = scopes.Pop();
        foreach (var variable in scope)
        {
            if (variable.Value.VarState != VarState.READ)
            {
                Lox.Warn(variable.Value.Token, $"Identifier '{variable.Value.Token.Lexeme}' is never used.");
            }
        }
    }
    
    private void Resolve(Stmt stmt) => stmt.Accept(this);
    private void Resolve(List<Stmt> statements) => statements.ForEach(Resolve);

    private void Resolve(Expr expr) => expr.Accept(this);
    private void Resolve(List<Expr> expr) => expr.ForEach(Resolve);
    
    private void ResolveLocal(Token name)
    {
        for (var i = 0; i < scopes.Count; i++)
        {
            if (scopes.ElementAt(i).TryGetValue(name.Lexeme, out var value))
            {
                value.VarState = VarState.READ;
                return;
            }
        }
        
        if(StandardLibraryList.StandardLibFunctions.ContainsKey(name.Lexeme)) return;
        Lox.Error(name, $"Can not resolve symbol '{name.Lexeme}'.");
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
    
    private class Variable(Token token)
    {
        public VarState VarState { get; set; } = VarState.DECLARED;

        public Token Token { get; } = token;
    }
    
    private enum FunctionType
    {
        NONE,
        FUNCTION,
        METHOD
    }
    
    private enum VarState
    {
        DECLARED,
        DEFINED,
        READ
    }
}