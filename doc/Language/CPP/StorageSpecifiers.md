

# static变量

> The *static initialization order fiasco* refers to the ambiguity in the order that objects with static storage duration in different translation units [are initialized](https://en.cppreference.com/w/cpp/language/initialization#Non-local_variables) in. If an object in one translation unit relies on an object in another translation unit already being initialized, a crash can occur if the compiler decides to initialize them in the wrong order. For example, the order in which .cpp files are specified on the command line may alter this order. The Construct on First Use Idiom can be used to avoid the static initialization order fiasco and ensure that all objects are initialized in the correct order.
>
> Within a single translation unit, the fiasco does not apply because the objects are initialized from top to bottom.
>
> 来自[cppreference](https://en.cppreference.com/w/cpp/language/siof#:~:text=The%20static%20initialization%20order%20fiasco,translation%20units%20are%20initialized%20in.)

# 参考资料

- [CPP Reference: Storage class specifiers](https://en.cppreference.com/w/cpp/language/storage_duration)

