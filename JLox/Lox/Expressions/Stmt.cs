// ReSharper disable once CheckNamespace
namespace Lox;

public abstract record Stmt
{
	public record Block(List<Stmt> Statements) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitBlockStmt(this);
		}
	}

	public record Expression(Expr Body) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitExpressionStmt(this);
		}
	}

	public record If(Expr Condition, Stmt ThenBranch, Stmt? ElseBranch) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitIfStmt(this);
		}
	}

	public record Print(Expr Right) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitPrintStmt(this);
		}
	}

	public record Var(Token Name, Expr? Initializer) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitVarStmt(this);
		}
	}

	public record While(Expr Condition, Stmt Body) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitWhileStmt(this);
		}
	}

	public record Break(Token Body) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitBreakStmt(this);
		}
	}

	public abstract T Accept<T>(IVisitor<T> visitor);

	public interface IVisitor<out T>
	{
		T VisitBlockStmt (Block stmt);
		T VisitExpressionStmt (Expression stmt);
		T VisitIfStmt (If stmt);
		T VisitPrintStmt (Print stmt);
		T VisitVarStmt (Var stmt);
		T VisitWhileStmt (While stmt);
		T VisitBreakStmt (Break stmt);
	}
}
