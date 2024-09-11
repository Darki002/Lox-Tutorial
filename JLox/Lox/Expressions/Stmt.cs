// ReSharper disable once CheckNamespace
namespace Lox;

public abstract record Stmt
{
	public record Expression(Expr Body) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitExpressionStmt(this);
		}
	}

	public record Print(Expr Right) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitPrintStmt(this);
		}
	}

	public record Var(Token Name, Expr Initializer) : Stmt
	{
		public override T Accept<T>(IVisitor<T> visitor)
		{
			return visitor.VisitVarStmt(this);
		}
	}

	public abstract T Accept<T>(IVisitor<T> visitor);

	public interface IVisitor<out T>
	{
		T VisitExpressionStmt (Expression stmt);
		T VisitPrintStmt (Print stmt);
		T VisitVarStmt (Var stmt);
	}
}
