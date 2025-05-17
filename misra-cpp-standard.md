# MISRA C++ Coding Standard Reference Guide

## Introduction to MISRA C++

MISRA (Motor Industry Software Reliability Association) C++ is a set of software development guidelines specifically designed for safety-critical embedded systems in the automotive industry. The standard promotes code quality, maintainability, portability, and reliability through a comprehensive set of rules and directives that restrict the use of error-prone language features.

### Purpose and Scope

MISRA C++ 2008 provides:
- 228 rules divided into mandatory, required and advisory classifications
- Compliance framework for safety-critical systems
- Support for ISO 26262 functional safety certification
- Focus on deterministic behavior and resource utilization

## Rule Classification System

MISRA C++ rules are classified according to their compliance requirements:

| Category | Symbol | Description |
|----------|--------|-------------|
| Mandatory | **M** | Rules that must not be violated under any circumstances. Deviations are not permitted. |
| Required | **R** | Rules that must be followed except in exceptional circumstances with formal documented justification. |
| Advisory | **A** | Recommended guidelines that should be followed when practical. |

## Core MISRA C++ Rule Categories

### 1. Language Extensions and Environment

Rules addressing compiler-specific extensions and environmental considerations.

| Rule | Classification | Description |
|------|----------------|-------------|
| 0-1-1 | **R** | A project shall not contain unreachable code. |
| 0-1-2 | **R** | A project shall not contain infeasible paths. |
| 0-1-3 | **R** | A project shall not contain unused variables. |
| 0-1-4 | **A** | A project should not contain unused functions. |
| 0-1-6 | **R** | The compile-time checking shall be used wherever possible. |
| 0-1-7 | **R** | The preprocessor shall only be used for file inclusion and simple macro definitions. |

### 2. Lexical Conventions

Rules governing the basic lexical elements like identifiers, comments, and character sets.

| Rule | Classification | Description |
|------|----------------|-------------|
| 2-3-1 | **R** | Trigraphs shall not be used. |
| 2-5-1 | **R** | Digraphs shall not be used. |
| 2-7-1 | **R** | The character sequence /* shall not be used within a C-style comment. |
| 2-7-2 | **R** | Sections of code shall not be commented out using C-style comments. |
| 2-7-3 | **A** | Sections of code should not be commented out using C++ comments. |
| 2-10-1 | **R** | Different identifiers shall be typographically unambiguous. |
| 2-10-2 | **R** | Identifiers declared in an inner scope shall not hide an identifier declared in an outer scope. |
| 2-10-3 | **A** | A typedef name should be a unique identifier. |
| 2-10-4 | **R** | A class, union or enum name shall be a unique identifier. |
| 2-13-1 | **R** | Only those escape sequences that are defined in ISO/IEC 14882:2003 shall be used. |

### 3. Standard Libraries and Extensions

Rules relating to the use of standard libraries and language extensions.

| Rule | Classification | Description |
|------|----------------|-------------|
| 3-1-1 | **R** | It shall be possible to include any header file in multiple translation units without violating the One Definition Rule. |
| 3-1-2 | **R** | Functions shall not be declared at block scope. |
| 3-1-3 | **R** | When an array is declared as external, its size shall be explicitly stated. |
| 3-2-1 | **R** | All declarations of an object or function shall have compatible types. |
| 3-2-2 | **R** | The One Definition Rule shall not be violated. |
| 3-2-3 | **R** | A type, object or function that is used in multiple translation units shall be declared in one and only one file. |
| 3-2-4 | **R** | An identifier with external linkage shall have exactly one definition. |
| 3-3-1 | **R** | Objects or functions with external linkage shall be declared in a header file. |
| 3-3-2 | **R** | If a function has internal linkage then all re-declarations shall include the static storage class specifier. |
| 3-4-1 | **R** | An identifier declared to be an object or type shall be defined in a block that minimizes its visibility. |

### 4. Memory Management

Rules addressing dynamic memory allocation and management.

| Rule | Classification | Description |
|------|----------------|-------------|
| 18-4-1 | **R** | Dynamic heap memory allocation shall not be used. |
| 18-0-1 | **R** | The C library functions calloc, malloc, realloc and free shall not be used. |
| 18-0-2 | **R** | The library functions atof, atoi and atol from library stdlib.h shall not be used. |
| 18-0-3 | **R** | The library functions abort, exit, getenv and system from library stdlib.h shall not be used. |
| 18-0-4 | **R** | The time handling functions of library time.h shall not be used. |
| 18-0-5 | **R** | The unbounded functions of library string.h shall not be used. |
| 18-7-1 | **R** | The signal handling facilities of signal.h shall not be used. |

### 5. Naming Conventions

Rules establishing consistent and meaningful naming patterns.

```cpp
// Non-compliant naming conventions
class logger {
private:
    std::shared_ptr<spdlog::logger> logger_;
    static std::shared_ptr<Logger> default_instance_;
    int MaxFileSize;
};

// MISRA-compliant naming conventions
class Logger_t {
private:
    std::shared_ptr<spdlog::logger> m_logger;
    static std::shared_ptr<Logger_t> ms_defaultInstance;
    static const std::int32_t k_maxFileSize = 10485760;
};
```

| Prefix/Suffix | Usage | Example |
|---------------|-------|---------|
| `_t` | Type definitions | `Logger_t` |
| `E_` | Enumeration types | `E_LogLevel` |
| `m_` | Non-static member variables | `m_logger` |
| `ms_` | Static member variables | `ms_defaultInstance` |
| `k_` | Constants | `k_maxFileSize` |

### 6. Integer Handling

Rules ensuring proper and consistent use of integer types.

| Rule | Classification | Description |
|------|----------------|-------------|
| 5-0-3 | **R** | An explicit integral or floating-point conversion shall not reduce the size of the underlying type. |
| 5-0-4 | **R** | An implicit integral conversion shall not change the signedness of the underlying type. |
| 5-0-5 | **R** | There shall be no implicit floating-point conversions from a type of greater precision to one of lesser precision. |
| 5-0-6 | **R** | An implicit integral or floating-point conversion shall not change the signedness of the underlying type. |
| 5-0-7 | **R** | There shall be no explicit floating-point conversion from a type of lesser precision to one of greater precision. |
| 5-0-8 | **R** | An explicit integral conversion shall not reduce the size of the underlying type if it is reasonable to expect the value to be out of range. |
| 5-0-9 | **R** | An explicit integral or floating-point conversion shall not increase the size of the underlying type of a signed expression. |
| 5-0-10 | **R** | If the bitwise operators ~ and << are applied to an operand with an underlying type that is wider than int, the result shall be explicitly cast to an appropriate type. |
| 5-0-11 | **R** | The plain char type shall only be used for the storage and use of character values. |
| 5-0-12 | **R** | Signed char and unsigned char type shall only be used for the storage and use of numeric values. |

```cpp
// Non-compliant integer handling
int get_int(const std::string& section, const std::string& key,
           int default_value = 0);

// MISRA-compliant integer handling
std::int32_t Get_Int32(const std::string& section, const std::string& key,
                      std::int32_t defaultValue = 0);
```

### 7. Error Handling

Rules governing exception handling and error propagation.

| Rule | Classification | Description |
|------|----------------|-------------|
| 15-0-1 | **R** | Exceptions shall only be used for error handling. |
| 15-0-2 | **A** | An exception object should not have pointer type. |
| 15-0-3 | **R** | Control shall not be transferred into a try or catch block using a goto or a switch statement. |
| 15-1-1 | **R** | The assignment-operator shall handle self-assignment correctly. |
| 15-1-2 | **A** | All classes should define a copy constructor, a copy assignment operator, and a destructor, whether they are used or not. |
| 15-1-3 | **R** | A copy constructor shall copy all base and member objects. |
| 15-3-1 | **R** | Exceptions shall be raised only after start-up and before termination of the program. |
| 15-3-2 | **A** | There should be at least one exception handler to catch all otherwise unhandled exceptions. |
| 15-3-3 | **R** | Handlers of a function-try-block implementation of a class constructor or destructor shall not reference non-static members from this class or its bases. |
| 15-3-4 | **R** | Each exception explicitly thrown in the code shall have a handler of a compatible type in all call paths that could lead to that point. |

```cpp
// Non-compliant error handling
bool load_from_file(const std::string& config_file = "/etc/vsnlogger.conf");

// MISRA-compliant error handling
enum class E_ConfigResult : std::uint8_t
{
    E_SUCCESS = 0U,
    E_FILE_NOT_FOUND,
    E_PERMISSION_DENIED,
    E_INVALID_FORMAT,
    E_UNKNOWN_ERROR
};

E_ConfigResult Load_FromFile(const std::string& configFile);
```

### 8. Classes and Inheritance

Rules addressing class structure, inheritance, and object-oriented design.

| Rule | Classification | Description |
|------|----------------|-------------|
| 10-1-1 | **A** | Classes should not be derived from virtual bases. |
| 10-1-2 | **R** | A base class shall not be both virtual and accessible to a derived class. |
| 10-1-3 | **R** | An accessible base class shall not be both virtual and inherited by another base class. |
| 10-2-1 | **A** | All accessible entity names within a multiple inheritance hierarchy should be distinguishable. |
| 10-3-1 | **R** | There shall be no more than one definition of each virtual function on each path through the inheritance hierarchy. |
| 10-3-2 | **R** | Each overriding virtual function shall be declared with the virtual keyword. |
| 10-3-3 | **R** | A virtual function shall only be overridden by a pure virtual function if it is itself declared as pure virtual. |

### 9. Templates

Rules governing the use of C++ templates and their instantiations.

| Rule | Classification | Description |
|------|----------------|-------------|
| 14-5-1 | **R** | A non-member generic function shall only be declared in a namespace that contains only the following: <ul><li>The non-member function declaration</li><li>Other template declarations</li><li>extern "C" declarations</li></ul> |
| 14-5-2 | **R** | A copy constructor shall be declared when there is a template constructor with a single parameter that is a parameterized type. |
| 14-5-3 | **R** | A copy assignment operator shall be declared when there is a template assignment operator with a parameter that is a parameterized type. |
| 14-6-1 | **R** | When an instantiated template contains a function or static member variable definition that is not dependent on a template parameter, it shall not imply a different definition than the one that would occur if the template was not included. |
| 14-6-2 | **R** | An explicit specialization of a function template shall be declared before the first use of the function in that translation unit. |
| 14-7-1 | **R** | For each template function, the types of all parameters in the function definition shall match the types in the template declaration. |
| 14-7-2 | **R** | A function shall not be declared as both inline and virtual. |
| 14-7-3 | **R** | The definition of a function template shall be in scope at the point where the template is instantiated. |

### 10. Control Flow and Statements

Rules addressing control flow structures and statements.

| Rule | Classification | Description |
|------|----------------|-------------|
| 6-2-1 | **R** | Assignment operators shall not be used in sub-expressions. |
| 6-2-2 | **R** | Floating-point expressions shall not be directly or indirectly tested for equality or inequality. |
| 6-2-3 | **R** | Before preprocessing, a null statement shall only occur on a line by itself, and shall not have any other text on the same line. |
| 6-3-1 | **M** | Statements shall always be reached. |
| 6-4-1 | **R** | An if ( condition ) construct shall be followed by a compound statement. The else keyword shall be followed by either a compound statement or another if statement. |
| 6-4-2 | **R** | All if ... else if constructs shall be terminated with an else clause. |
| 6-4-3 | **R** | A switch statement shall be a well-formed switch statement. |
| 6-4-4 | **R** | A switch-label shall only be used when the most closely enclosing compound statement is the body of a switch statement. |
| 6-4-5 | **R** | An unconditional throw or break statement shall terminate every non-empty switch-clause. |
| 6-4-6 | **R** | The final clause of a switch statement shall be the default-clause. |
| 6-4-7 | **R** | The condition of a switch statement shall not have bool type. |
| 6-4-8 | **R** | Every switch statement shall have at least one case-clause. |
| 6-5-1 | **R** | A for loop shall contain a single loop-counter which shall not have floating-point type. |
| 6-5-2 | **R** | If loop-counter is not modified by -- or ++, then, within the body of the loop, the loop-counter shall only be modified by the expansion of the for-expression. |
| 6-5-3 | **R** | The loop-counter shall not be modified within the body of the for loop. |
| 6-5-4 | **R** | The loop-counter shall be modified by one of: --, ++, -= n, or += n; where n remains constant for the duration of the loop. |
| 6-5-5 | **R** | A loop-control-variable shall not be modified within the body of a do...while or a while loop. |
| 6-5-6 | **R** | A loop-control-variable other than the loop-counter shall not be modified within the body of a for loop. |
| 6-6-1 | **R** | Any statement with a label shall be a loop statement or a switch statement. |
| 6-6-2 | **R** | The goto statement shall jump to a label declared later in the same function body. |
| 6-6-3 | **R** | The continue statement shall only be used within a well-formed for, do...while, or while loop. |
| 6-6-4 | **R** | For any iteration statement there shall be at most one break or continue statement. |
| 6-6-5 | **R** | A function shall have a single point of exit at the end of the function. |

## Implementation Considerations for Automotive Systems

### Memory Resource Management

The automotive industry imposes strict requirements on memory usage due to constraints in embedded environments:

```cpp
// Non-compliant implementation with unconstrained memory allocation
std::shared_ptr<spdlog::sinks::sink> create_console_sink(bool colored) {
    if (colored) {
        return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    } else {
        return std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }
}

// MISRA C++ compliant implementation with deterministic allocation boundaries
std::shared_ptr<spdlog::sinks::sink> Create_ConsoleSink(bool colored)
{
    static std::uint32_t s_allocationCount = 0U;
    
    /* Enforce allocation limits */
    if (s_allocationCount >= MAX_SINK_ALLOCATIONS)
    {
        return nullptr;
    }
    
    std::shared_ptr<spdlog::sinks::sink> result = nullptr;
    
    if (colored)
    {
        result = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }
    else
    {
        result = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }
    
    if (result != nullptr)
    {
        ++s_allocationCount;
    }
    
    return result;
}
```

### Boundary Verification Requirements

Comprehensive bounds checking for all array operations:

```cpp
// Non-compliant boundary checking
for (const auto& field : additional_fields) {
    json << ",\"" << field.first << "\":\"";
    for (char c : field.second) {
        if (c == '\"')
            json << "\\\"";
        else if (c == '\\')
            json << "\\\\";
        else
            json << c;
    }
    json << "\"";
}

// MISRA C++ compliant boundary checking
for (std::size_t i = 0U; i < additional_fields.size(); ++i)
{
    const auto& field = additional_fields[i];
    
    json << ",\"";
    
    /* Enforce field name size constraints */
    const std::size_t k_maxKeyLength = 64U;
    const std::size_t actualKeyLength = (field.first.length() < k_maxKeyLength) ? 
                                         field.first.length() : k_maxKeyLength;
    
    for (std::size_t j = 0U; j < actualKeyLength; ++j)
    {
        json << field.first[j];
    }
    
    json << "\":\"";
    
    /* Enforce field value size constraints */
    const std::size_t k_maxValueLength = 256U;
    const std::size_t actualValueLength = (field.second.length() < k_maxValueLength) ? 
                                           field.second.length() : k_maxValueLength;
    
    for (std::size_t j = 0U; j < actualValueLength; ++j)
    {
        char c = field.second[j];
        if (c == '\"')
        {
            json << "\\\"";
        }
        else if (c == '\\')
        {
            json << "\\\\";
        }
        else
        {
            json << c;
        }
    }
    
    json << "\"";
}
```

### Deterministic Execution Requirements

MISRA C++ demands deterministic behavior for safety-critical systems:

1. **Stack Usage Analysis**: Determine maximum stack consumption for all execution paths
2. **Timing Analysis**: Calculate worst-case execution time (WCET) for all critical functions
3. **Memory Allocation**: Use static allocation patterns instead of dynamic allocation
4. **Concurrency Control**: Explicit synchronization with bounded waiting times

## Verification and Compliance Assessment

### Static Analysis Tools

Several specialized tools support MISRA C++ compliance verification:

1. **Parasoft C/C++test**: Comprehensive static analysis with MISRA C++ rule checking
2. **LDRA Testbed**: Static and dynamic analysis with MISRA compliance reporting
3. **PC-lint/FlexeLint**: Lightweight but powerful static analyzers
4. **Coverity**: Defect detection with MISRA compliance checking
5. **Helix QAC**: Advanced static analysis specifically designed for embedded systems

### Compiler Flags for MISRA Enforcement

Modern compilers provide warning flags that help enforce MISRA compliance:

```bash
# GCC compiler flags for MISRA C++ enforcement
CXXFLAGS += \
    -Wall \
    -Wextra \
    -Werror \
    -Wconversion \
    -Wsign-conversion \
    -Wcast-align \
    -Wcast-qual \
    -Wformat=2 \
    -Wold-style-cast \
    -Wredundant-decls \
    -Wshadow \
    -Wstrict-overflow=5 \
    -Wswitch-default \
    -Wundef \
    -Wunused \
    -Woverloaded-virtual \
    -Wpedantic
```

### Deviation Management Protocol

MISRA C++ acknowledges that sometimes rules must be violated. A formal deviation management process is required:

1. **Deviation Request**: Formal documentation of the need to violate a rule
2. **Risk Assessment**: Analysis of potential impact on safety and reliability
3. **Mitigation Strategy**: Methods to reduce risk associated with the deviation
4. **Approval Process**: Sign-off by designated authority
5. **Documentation**: Recording of approved deviations in compliance documentation

Example Deviation Record:

```
DEVIATION RECORD
----------------
Rule: MISRA C++ Rule 18-4-1 (Dynamic heap memory allocation shall not be used)
Location: logger_t.cpp:157-164
Justification: Dynamic allocation required for logger initialization during startup phase only
Risk Assessment: Limited to initialization phase, with maximum allocation bounds enforced
Mitigation: Resource monitoring, allocation counting, and failure detection implemented
Approved By: J. Smith, Safety Engineer
Date: 2025-03-15
```

## Functional Safety Integration

### ISO 26262 Compliance Requirements

MISRA C++ serves as a foundation for achieving automotive functional safety certification:

1. **ASIL Classification**: Safety requirements based on Automotive Safety Integrity Level (ASIL A-D)
2. **Verification Methods**: Test coverage requirements based on ASIL level
3. **Tool Qualification**: Verification that development tools don't introduce errors
4. **Traceability**: Documentation linking requirements to implementation and tests

### Coding Standard Enforcement Workflow

Recommended workflow for MISRA C++ compliance in automotive development:

1. **Developer Education**: Training on MISRA C++ rules and principles
2. **IDE Integration**: Real-time compliance checking during development
3. **Pre-commit Validation**: Automated compliance checking before version control commits
4. **Continuous Integration**: Automated validation in the CI/CD pipeline
5. **Formal Code Reviews**: Manual inspection focusing on safety-critical components
6. **Compliance Documentation**: Generated reports for certification evidence

## References and Standards

1. MISRA. (2008). *MISRA C++:2008 - Guidelines for the use of the C++ language in critical systems*. MIRA Limited.

2. ISO/IEC. (2014). *ISO 26262:2018 - Road vehicles — Functional safety*. International Organization for Standardization.

3. Koopman, P. (2010). *Better Embedded System Software*. Drumnadrochit Education LLC.

4. Stroustrup, B. (2013). *The C++ Programming Language* (4th ed.). Addison-Wesley.

5. Douglass, B. P. (2010). *Real-Time Agility: The Harmony/ESW Method for Real-Time and Embedded Systems Development*. Addison-Wesley.

6. Hatton, L. (2004). *Safer C: Developing Software for High-Integrity and Safety-Critical Systems*. McGraw-Hill.

7. Montgomery, P. Y., & Holzmann, G. J. (2014). *Static Analysis of Embedded Automotive Software*. In Automotive Software Engineering (pp. 191-214). Springer.
