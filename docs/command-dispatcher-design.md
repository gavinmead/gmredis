# Command Dispatcher Architecture

## Overview

The command dispatcher system is responsible for parsing RESP protocol arrays, selecting the appropriate command handler, and executing commands against the key-value store.

## Design Patterns

### Command Pattern
Each Redis command (GET, SET, PING, etc.) is implemented as a concrete class inheriting from a base `Command` class. This provides:
- **Extensibility**: Easy to add new commands without modifying existing code
- **Separation of concerns**: Each command encapsulates its own validation and execution logic
- **Type safety**: Strongly-typed parameters specific to each command's needs

### Template Method Pattern
The base `Command` class defines the command execution lifecycle using the Template Method pattern:

1. **Validation** - Verify command arguments are correct
2. **Execution** - Perform the command operation
3. **Hooks** - Optional extension points for cross-cutting concerns

**Hook Examples:**
- Pre-execution: authorization, rate limiting, dry-run mode
- Post-execution: logging, metrics, cache invalidation
- Error handling: custom error responses per command
- Transaction support: multi-command transactions (future)

All hooks have placeholder implementations in the base class and can be optionally overridden by concrete commands as needed.

## Architecture Components

### 1. Command (Base Class + Subclasses)

**Location:** `lib/gmredis/include/gmredis/command/`

**Responsibilities:**
- Define the command execution lifecycle (Template Method)
- Provide optional hooks with default implementations
- Declare pure virtual methods for validation and execution

**Concrete Commands:**
- `PingCommand`
- `GetCommand`
- `SetCommand`
- Additional commands as needed

**Characteristics:**
- Commands are **stateless** - they operate on the KV store, not internal state
- Commands are **singletons** - one instance per command type
- Commands implement their specific validation and execution logic

### 2. CommandRegistry

**Responsibilities:**
- Maintain the registry of all available commands
- Instantiate and manage singleton command instances
- Provide lookup functionality based on command type (enum)

**Key Design Decision:**
Uses an `enum class` for command type identification to provide **compile-time safety**:
- Can't register typo'd command names
- Exhaustive switch statements warn if cases are missed
- Refactoring is safer (rename enum, compiler shows all usage)
- IDE autocomplete support

### 3. CommandSelector

**Responsibilities:**
- Parse the first element of a RESP Array to identify the command
- Map string command names to the command type enum
- Handle case-insensitivity (GET, get, Get all map to CommandType::GET)
- Retrieve the appropriate Command instance from CommandRegistry
- Handle errors for unknown or invalid commands

**Flow:**
```
RESP Array → CommandSelector → String-to-Enum mapping →
CommandRegistry lookup → Command instance → Execute
```

## Execution Flow

1. **Parse RESP Array**: Server receives and parses incoming RESP array
2. **Command Selection**: CommandSelector examines first array element
3. **String→Enum Mapping**: Convert string command name to enum (case-insensitive)
4. **Registry Lookup**: CommandRegistry returns singleton Command instance for enum
5. **Validation**: Command validates remaining array elements (arguments)
6. **Execution**: Command executes against KV store
7. **Hooks**: Optional pre/post execution hooks fire
8. **Response**: Command returns serialized RESP response

## Error Handling

**CommandSelector is responsible for:**
- Unknown command errors (command not in registry)
- Invalid command format errors (malformed RESP array)

**Individual Commands are responsible for:**
- Argument validation errors (wrong number of args, wrong types)
- Execution errors (key not found, wrong value type, etc.)

## Design Trade-offs

### Enum-based Registry (Chosen Approach)

**Advantages:**
- Compile-time safety
- Exhaustive switch checking
- Refactoring safety
- Clear command inventory
- IDE support

**Trade-offs:**
- Extra mapping step (string → enum → command)
- Cannot dynamically add commands at runtime (acceptable for Redis implementation)
- Case-insensitive mapping must be implemented in CommandSelector

### Alternative: String-based Registry (Not Chosen)

**Advantages:**
- Direct string lookup (simpler)
- No mapping layer needed

**Trade-offs:**
- No compile-time safety
- Typos can cause runtime bugs
- No exhaustive checking

**Decision:** Compile-time safety was prioritized, especially as the command set grows toward Redis's 200+ commands.

## Directory Structure

```
lib/gmredis/
├── include/gmredis/
│   └── command/
│       ├── command.h           # Base Command class
│       ├── command_registry.h  # CommandRegistry
│       ├── command_selector.h  # CommandSelector
│       ├── command_types.h     # CommandType enum class
│       ├── ping_command.h      # Concrete command
│       ├── get_command.h       # Concrete command
│       └── set_command.h       # Concrete command
└── src/command/
    ├── command.cpp
    ├── command_registry.cpp
    ├── command_selector.cpp
    ├── ping_command.cpp
    ├── get_command.cpp
    └── set_command.cpp
```

## Future Considerations

- **Command aliasing**: Support command aliases (e.g., QUIT → SHUTDOWN)
- **Command versioning**: Handle different Redis protocol versions
- **Pipelining**: Batch command execution
- **Transactions**: MULTI/EXEC support
- **Pub/Sub**: Special command handling for pub/sub operations
- **Modules**: Plugin system for custom commands
- **Command statistics**: Track command usage, latency, errors

## Integration with Existing Code

The command dispatcher integrates with:
- **RESP Protocol Parser** (`lib/gmredis/protocol/parse.*`): Parses incoming requests into RESP arrays
- **RESP Serializer** (`lib/gmredis/protocol/serialize.*`): Serializes command responses
- **KV Store** (`lib/gmredis/storage/`): Commands execute operations against the store

## Testing Strategy

- **Unit tests**: Each command's validation and execution logic
- **Integration tests**: Full flow from RESP parsing → command execution → response
- **Registry tests**: Command registration and lookup
- **Selector tests**: String-to-enum mapping, case-insensitivity, error handling
- **Template method tests**: Verify hook execution order and optional overrides