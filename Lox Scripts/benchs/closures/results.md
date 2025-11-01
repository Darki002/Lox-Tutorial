# Old Style

## Upvalues

0.37799999999999989

## no Upvalues

0.17099999999999999

# Only closures if needed

## Upvalues

0.33900000000000019

## no Upvalues

0.20899999999999999

# Result

Remove the change, since it gives only a little when stress testing closures. But for thing like the
fib script, it slows down.