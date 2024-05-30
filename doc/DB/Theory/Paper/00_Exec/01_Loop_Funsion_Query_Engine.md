push versus pull based loop fusion in query engines

# Loop fusion in collection programming
## Fold Fusion
> In this approach, every collection operation is implemented using two constructs: (1) the build method for producing a collection, and (2) the foldr method for consuming a collection.

## Unfold Funsion
> Every collection operation is expressed in terms of the two constructs unfold and destroy. The destroy method consumes the given list. The generate function generates a collection whose elements are specified by the input function passed to this method.


# 4. Loop fusion is operator pipelining
> Operators can be divided into three categories: (1) The operators responsible for producing a collection from a given source (e.g., a file or an array), (2) the operators which transform the given collection to another collection, and (3) the consumer operators which aggregate the given collection into a single result.

| Pipelined  Query engines | Object-oriented Design pattern |                    Collection Loop fusion                    |
| :----------------------: | :----------------------------: | :----------------------------------------------------------: |
|       Pull Engine        |            Iterator            | Unfold fusion (Svenningsson, 2002)<br/>Stream fusion (Coutts et al., 2007) |
|       Push Engine        |            Visitor             |               Fold fusion (Gill et al., 1993)                |



# 5. An improved pull-based engine

## Stream Fusion
问题：
> In functional languages, loops are expressed as recursive functions. Reasoning about recursive functions is very hard for optimizing compilers.

解决方案:
> Stream fusion tries to solve this issue by converting all recursive collection operations to non-recursive stream operations. To do so, first all collections are converted to streams using the stream method. Then, the corresponding method on the stream is invoked that results in a transformed stream. Finally, the transformed stream is converted back to a collection by invoking the unstream method.

## Stream-fusion engine

> The stream-fusion engine combines the benefits of both engines by providing the following **two constructs for early termination of loops and skipping an iteration**. First, the Done construct denotes the termination of loops, and in essence has the same effect as the break construct in an imperative language like C. Second, the Skip construct results in skipping to the next iteration, and has an equivalent effect to the continue construct in an imperative language like C

| Pipelined query engines |    Looping constructs    |
| :---------------------: | :----------------------: |
|       Push engine       |     while + continue     |
|       Pull engine       |      while + break       |
|  Stream-fusion engine   | while + break + continue |

# 6. Implementation

## Removing intermediate results

> Although the stream-fusion engine removes intermediate relations, it creates intermediate Step objects. There are two problems with these intermediate objects. 
>
> - First, the Step data type operations are virtual calls. This causes poor cache locality and degrades performance
> - Second, normally these intermediate objects lead to heap allocations. This causes higher memory consumption and worse running times. 



> The first problem with virtual calls can be solved by rewriting the Step operations by enumerating all cases for the Step object.

> The good news is that these heap allocations can be converted to stack allocations. This is because the created objects are not escaping their usage scope.



> the stream fusion engine implements a pull engine on a coarse-grained level (i.e., relation level) and pushes the individual elements on a fine-grained level (i.e., tuple level).
>
> By comparing this code to the code produced by a push engine, we see a clear similarity. First, there are no more additional virtual calls associated with the Step operators. Second, there is no more materialization of the intermediate Step objects. Finally, similar to push engines, the produced code does not contain any additional nested while loop for selection.

# 8 Discussion: Column stores and vectorization

> The performance of column-store engines is better than the performance of the row-store counterparts. Furthermore, the relative speedup of different engines over a pull-based engine using the columnar layout representation is similar to the speedup of the corresponding engines over a pull-based engine for the row layout representation
