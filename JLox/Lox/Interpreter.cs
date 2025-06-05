using System.Globalization;
using Lox.Callables;
using Lox.Callables.StandardLibrary;
using Lox.Errors;
using Lox.Helpers;

namespace Lox;

public class Interpreter : Expr.IVisitor<object?>, Stmt.IVisitor<Void?>
{
    private Environment? environment;
    
    private readonly Dictionary<string, object?> globals = new ();
    private readonly Dictionary<Expr, int> locals = [];
    private readonly Dictionary<Expr, int> indexes = [];

    public Interpreter()
    {
        foreach (var (key, value) in StandardLibraryList.StandardLibFunctions)
        {
            globals.Add(key, value);
        }
    }
    
    public void Interpret(List<Stmt> statements)
    {
        try
        {
            foreach (var stmt in statements)
            {
                Execute(stmt);
            }
        }
        catch (RuntimeError e)
        {
            Lox.RuntimeError(e);
        }
    }

    public object? VisitAssignExpr(Expr.Assign expr)
    {
        var value = Evaluate(expr.Value);

        if (locals.TryGetValue(expr, out var dist))
        {
            environment!.AssignAt(dist, indexes[expr], value);
        }
        else if(globals.ContainsKey(expr.Name.Lexeme))
        {
            globals[expr.Name.Lexeme] = value;
        }
        else
        {
            throw new RuntimeError(expr.Name, $"Undefined variable '{expr.Name.Lexeme}'.");
        }
        
        return value;
    }

    public object? VisitBinaryExpr(Expr.Binary expr)
    {
        var right = Evaluate(expr.Right);
        var left = Evaluate(expr.Left);

        switch (expr.Operator.Type)
        {
            case TokenType.STAR:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! * (double)right!;
            case TokenType.SLASH:
                CheckNumberOperand(expr.Operator, left, right);
                if ((double)right! != 0) return (double)left! / (double)right;
                throw new RuntimeError(expr.Operator, "Division by Zero.");
            case TokenType.MINUS:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! - (double)right!;
            case TokenType.PLUS:
                return left switch
                {
                    double leftNum when right is double rightNum => leftNum + rightNum,
                    string leftStr when right is string rightStr => leftStr + rightStr,
                    double leftNumToStr when right is string rightNumStr => leftNumToStr + rightNumStr,
                    string leftNumStr when right is double rightNumToStr => leftNumStr + rightNumToStr,
                    _ => throw new RuntimeError(expr.Operator, "Operands must be two numbers or two strings.")
                };

            case TokenType.GREATER:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! > (double)right!;
            case TokenType.GREATER_EQUAL:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! >= (double)right!;
            case TokenType.LESS:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! < (double)right!;
            case TokenType.LESS_EQUAL:
                CheckNumberOperand(expr.Operator, left, right);
                return (double)left! <= (double)right!;
            case TokenType.EQUAL_EQUAL:
                return IsEqual(left, right);
            case TokenType.BANG_EQUAL:
                return !IsEqual(left, right);
        }

        return null; // Unreachable
    }

    public object? VisitGroupingExpr(Expr.Grouping expr)
    {
        return Evaluate(expr);
    }

    public object? VisitLiteralExpr(Expr.Literal expr)
    {
        return expr.Value;
    }

    public object? VisitLogicalExpr(Expr.Logical expr)
    {
        var left = Evaluate(expr.Left);

        if (expr.Operator.Type == TokenType.OR)
        {
            if (IsTruthy(left)) return left;
        }
        else
        {
            if (!IsTruthy(left)) return left;
        }

        return Evaluate(expr.Right);
    }

    public object? VisitSetExpr(Expr.Set expr)
    {
        var obj = Evaluate(expr.Obj);

        if (obj is not LoxInstance instance)
        {
            throw new RuntimeError(expr.Name, "Only instances have fields.");
        }

        var value = Evaluate(expr.Value);
        instance.Set(expr.Name, value);
        return value;
    }

    public object? VisitIndexSetExpr(Expr.IndexSet expr)
    {
        var obj = Evaluate(expr.Obj);

        if (obj is ArrayInstance array)
        {
            var index = Evaluate(expr.Index);
            if (index is double i)
            {
                var value = Evaluate(expr.Value);
                array.SetValue(i, value);
                return null;
            }

            throw new RuntimeError(expr.Token, "Expected value of type number for index set after '['");
        }
        
        throw new RuntimeError(expr.Token, "Can only access arrays with '[]'");
    }

    public object VisitSuperExpr(Expr.Super expr)
    {
        var distance = locals[expr];
        var superclass = (LoxClass)environment!.GetAt(distance, 0)!;
        var obj = (LoxInstance)environment.GetAt(distance - 1, 0)!;

        var method =superclass.FindMethod(expr.Method.Lexeme);

        if (method is null)
        {
            throw new RuntimeError(expr.Method, $"Undefined property '{expr.Method.Lexeme}'.");
        }
        
        return method.Bind(obj);
    }

    public object? VisitThisExpr(Expr.This expr)
    {
        return LookUpVariable(expr.Keyword, expr);
    }

    public object? VisitUnaryExpr(Expr.Unary expr)
    {
        var right = Evaluate(expr.Right);

        switch (expr.Operator.Type)
        {
            case TokenType.BANG:
                return !IsTruthy(right);
            case TokenType.MINUS:
                CheckNumberOperand(expr.Operator, right);
                return -(double)right!;
            default:
                return null; // Unreachable (Unary always have BANG or MINUS as the Operator)
        }
    }

    public object? VisitCallExpr(Expr.Call expr)
    {
        var callee = Evaluate(expr.Callee);
        List<object?> arguments = [];
        
        foreach (var argument in expr.Arguments)
        {
            arguments.Add(Evaluate(argument));
        }

        if (callee is not ILoxCallable function)
        {
            throw new RuntimeError(expr.Paren, "Can only call functions and classes.");
        }

        if (function.Arity != arguments.Count)
        {
            throw new RuntimeError(expr.Paren, $"Expected {function.Arity} arguments but got {arguments.Count}.");
        }

        return function.Call(this, arguments);
    }

    public object? VisitGetExpr(Expr.Get expr)
    {
        var obj = Evaluate(expr.Obj);
        if (obj is LoxInstance instance)
        {
            var result = instance.Get(expr.Name);
            if (result is LoxFunction function)
            {
                return function.IsGetter ? function.Call(this) : function;
            }
            return result;
        }
        
        throw new RuntimeError(expr.Name, "Only instances have properties.");
    }

    public object? VisitIndexGetExpr(Expr.IndexGet expr)
    {
        var obj = Evaluate(expr.Obj);

        if (obj is ArrayInstance array)
        {
            var index = Evaluate(expr.Index);
            if (index is double i) return array.GetValue(i);
            throw new RuntimeError(expr.Token, "Expected value of type number for index set after '['");
        }
        
        throw new RuntimeError(expr.Token, "Can only access arrays with '[]'");
    }

    public object? VisitVariableExpr(Expr.Variable expr)
    {
        return LookUpVariable(expr.Name, expr);
    }

    public object VisitFunctionExpr(Expr.Function expr)
    {
        return new LoxAnonymousFunction(expr, environment);
    }

    public object VisitArrayExpr(Expr.Array expr)
    {
        var items = new object?[expr.Items.Count];
        for (var i = 0; i < expr.Items.Count; i++)
        {
            items[i] = Evaluate(expr.Items[i]);
        }

        return new ArrayInstance(items);
    }

    public Void? VisitBlockStmt(Stmt.Block stmt)
    {
        ExecuteBlock(stmt.Statements, new Environment(environment));
        return null;
    }

    public Void? VisitClassStmt(Stmt.Class stmt)
    {
        object? superclass = null;
        if (stmt.Superclass is not null)
        {
            superclass = Evaluate(stmt.Superclass);
            if (superclass is not LoxClass)
            {
                throw new RuntimeError(stmt.Superclass.Name, "Superclass must be a class.");
            }
        }
        
        globals.Add(stmt.Name.Lexeme, null);

        if (stmt.Superclass is not null)
        {
            environment = new Environment(environment);
            environment.Define(superclass);
        }

        var classMethods = new Dictionary<string, LoxFunction>();
        foreach (var method in stmt.ClassMethods)
        {
            var function = new LoxFunction(method, environment, false);
            classMethods[method.Name.Lexeme] = function;
        }
        
        var metaClass = new LoxClass(null, null, $"{stmt.Name.Lexeme} metaclass", classMethods);

        var methods = new Dictionary<string, LoxFunction>();
        foreach (var method in stmt.Methods)
        {
            var function = new LoxFunction(method, environment, method.Name.Lexeme == "init");
            methods[method.Name.Lexeme] = function;
        }
        
        var klass = new LoxClass(metaClass, (LoxClass?)superclass, stmt.Name.Lexeme, methods);

        if (stmt.Superclass is not null)
        {
            environment = environment?.Enclosing;
        }
        
        globals[stmt.Name.Lexeme] = klass;
        return null;
    }

    public Void? VisitExpressionStmt(Stmt.Expression stmt)
    {
        Evaluate(stmt.Body);
        return null;
    }

    public Void? VisitFunctionStmt(Stmt.Function stmt)
    {
        var function = new LoxFunction(stmt, environment, false);
        Define(stmt.Name, function);
        return null;
    }

    public Void? VisitIfStmt(Stmt.If stmt)
    {
        if (IsTruthy(Evaluate(stmt.Condition)))
        {
            Execute(stmt.ThenBranch);
        }
        else if (stmt.ElseBranch is not null)
        {
            Execute(stmt.ElseBranch);
        }

        return null;
    }

    public Void? VisitPrintStmt(Stmt.Print stmt)
    {
        var value = Evaluate(stmt.Right);
        Console.WriteLine(Stringify(value));
        return null;
    }

    public Void? VisitReturnStmt(Stmt.Return stmt)
    {
        var value = stmt.Value != null ? Evaluate(stmt.Value) : null;
        throw new Return(value);
    }

    public Void? VisitVarStmt(Stmt.Var stmt)
    {
        object? initializer = null;
        if (stmt.Initializer is not null)
        {
            initializer = Evaluate(stmt.Initializer);
        }
        
        Define(stmt.Name, initializer);
        return null;
    }

    public Void? VisitWhileStmt(Stmt.While stmt)
    {
        while (IsTruthy(Evaluate(stmt.Condition)))
        {
            try
            {
                Execute(stmt.Body);
            }
            catch (Break)
            {
                break;
            }
        }

        return null;
    }

    public Void? VisitBreakStmt(Stmt.Break stmt)
    {
        throw new Break();
    }

    private object? Evaluate(Expr expr)
    {
        return expr.Accept(this);
    }

    private void Execute(Stmt stmt)
    {
        stmt.Accept(this);
    }
    
    public void ExecuteBlock(List<Stmt> stmts, Environment env)
    {
        var previous = environment;
        try
        {
            environment = env;
            foreach (var stmt in stmts)
            {
                Execute(stmt);
            }
        }
        finally
        {
            environment = previous;
        }
    }

    private static bool IsTruthy(object? right)
    {
        return right switch
        {
            null => false,
            bool boolean => boolean,
            _ => true
        };
    }

    private static bool IsEqual(object? right, object? left)
    {
        if (right is null && left is null) return true;
        if (right is null) return false;
        return right.Equals(left);
    }

    private static void CheckNumberOperand(Token @operator, object? operand)
    {
        if (operand is double) return;
        throw new RuntimeError(@operator, "Operand must be a number.");
    }

    private static void CheckNumberOperand(Token @operator, object? right, object? left)
    {
        if (right is double && left is double) return;
        throw new RuntimeError(@operator, "Operand must be a number.");
    }

    public static string? Stringify(object? obj)
    {
        if (obj == null) return "nil";

        if (obj is double num)
        {
            var text = num.ToString(CultureInfo.InvariantCulture);
            if (text.EndsWith(".0")) text = text.Sub(0, text.Length - 2);
            return text;
        }

        return obj.ToString();
    }

    public void Resolve(Expr expr, int depth, int index)
    {
        locals.Add(expr, depth);
        indexes.Add(expr, index);
    }

    private object? LookUpVariable(Token name, Expr expr)
    {
        if (locals.TryGetValue(expr, out var distance))
        {
            return environment!.GetAt(distance, indexes[expr]);
        }

        if (globals.TryGetValue(name.Lexeme, out var value))
        {
            return value;
        }
        
        throw new RuntimeError(name, $"Undefined variable '{name.Lexeme}'.");
    }

    private void Define(Token name, object? value)
    {
        if (environment is not null)
        {
            environment.Define(value);
        }
        else
        {
            globals.Add(name.Lexeme, value);
        }
    }
}