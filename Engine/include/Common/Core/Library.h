#pragma once

/**
 * A library for managing objects.
 *
 * The `Library` class provides functionality to add, load, retrieve, and check
 * for the existence of objects within the library. Each object is associated with
 * a unique name.
 *
 * @tparam ObjectType The type of object to be managed by the library.
 * @tparam OwnershipType The type of ownership for the objects (either direct or shared pointer).
 */
template<typename ObjectType, typename OwnershipType = std::shared_ptr<ObjectType>>
class Library
{
public:
    // Constructor/Destructor
    // ----------------------------------------
    /// @brief Create a new library.
    Library(const std::string& name = "Object") : m_ObjectsName(name) {}
    /// @brief Delete the library.
    virtual ~Library() = default;
    
    // Add
    // ----------------------------------------
    /// @brief Adds an object to the library.
    /// @param name The name to associate with the object.
    /// @param object The object to add.
    /// @note If an object with the same name already exists in the library, an assertion failure
    /// will occur.
    virtual void Add(const std::string& name,
                     const ObjectType& object)
    {
        bool exists = Exists(name);
        if (exists)
            CORE_WARN("{0} already exists!", GetName());
        else
            m_Objects[name] = object;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves an object from the library by its name.
    /// @param name The name of the object to retrieve.
    /// @return The retrieved object.
    /// @note If the object with the specified name does not exist in the library, an assertion
    /// failure will occur.
    ObjectType& Get(const std::string& name)
    {
        bool exists = Exists(name);
        if (!exists)
            CORE_WARN("{0} not found!", GetName());
        
        return m_Objects[name];
    }
    /// @brief Updates the object with the specific name.
    /// @param name The name to associate with the object.
    /// @param object The object to add.
    /// @note If the object with the specified name does not exist in the library, an assertion
    /// failure will occur.
    void Update(const std::string& name,
                const ObjectType& object)
    {
        bool exists = Exists(name);
        if (exists)
            m_Objects[name] = object;
        else
            CORE_WARN("{0} not found!", GetName());
    }
    /// @brief Checks if an object with a given name exists in the library.
    /// @param name The name of the object to check for existence.
    /// @return True if an object with the specified name exists in the library, otherwise false.
    bool Exists(const std::string& name) const
    {
        return m_Objects.find(name) != m_Objects.end();
    }
    
    // Iteration support
    // ----------------------------------------
    /// @brief Get the begin iterator for the library.
    /// @return Iterator pointing to the begin of the library.
    typename std::unordered_map<std::string, ObjectType>::iterator begin()
    {
        return m_Objects.begin();
    }
    /// @brief Get the end iterator for the library.
    /// @return Iterator pointing to the end of the library.
    typename std::unordered_map<std::string, ObjectType>::iterator end()
    {
        return m_Objects.end();
    }
    /// @brief Get the begin iterator for the library (constant value).
    /// @return Iterator pointing to the begin of the library.
    typename std::unordered_map<std::string, ObjectType>::const_iterator begin() const
    {
        return m_Objects.begin();
    }
    /// @brief Get the end iterator for the library (constant value).
    /// @return Iterator pointing to the end of the library.
    typename std::unordered_map<std::string, ObjectType>::const_iterator end() const
    {
        return m_Objects.end();
    }
    
protected:
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the name of the objects that are contained in the library.
    /// @return The name of the objects.
    const std::string& GetName() const { return m_ObjectsName; }
    
    // Library variables
    // ----------------------------------------
private:
    ///< A map of object names to their corresponding objects.
    std::unordered_map<std::string, ObjectType> m_Objects;
    
    ///< The name of the objects contained in the library.
    std::string m_ObjectsName;
};
