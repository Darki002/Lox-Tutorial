using Lox.Tree;

namespace Lox;

public class Analyzer: Expr.IVisitor<Void?>, Stmt.IVisitor<Void?>
{
    public Void? VisitAssignExpr(Expr.Assign expr)
    {
        throw new NotImplementedException();
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
        throw new NotImplementedException();
    }

    public Void? VisitFunctionExpr(Expr.Function expr)
    {
        throw new NotImplementedException();
    }

    public Void? VisitBlockStmt(Stmt.Block stmt)
    {
        throw new NotImplementedException();
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
        throw new NotImplementedException();
    }

    public Void? VisitWhileStmt(Stmt.While stmt)
    {
        throw new NotImplementedException();
    }

    public Void? VisitBreakStmt(Stmt.Break stmt)
    {
        throw new NotImplementedException();
    }
}