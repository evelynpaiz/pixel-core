#pragma once

namespace utils {
/**
 * Splits a string into two parts based on a delimiter.
 *
 * @param combinedString The string to split.
 * @param delimiter The character used as the delimiter.
 *
 * @return The splitted string in two parts.
 */
inline std::pair<std::string, std::string> SplitString(const std::string& combinedString,
                                                       char delimiter = '.') {
  size_t delimiterPos = combinedString.find(delimiter);

  // No need for a separate 'else' block - simplifies the logic
  return (delimiterPos != std::string::npos)
           ? std::make_pair(combinedString.substr(0, delimiterPos),
                            combinedString.substr(delimiterPos + 1))
           : std::make_pair(combinedString, "");
}

/**
 * Merges two strings into a single string, using a delimiter if the second string
 * is not empty.
 *
 * @param group The first part of the string.
 * @param member The second part of the string.
 * @param delimiter The delimiter to use if the second string is not empty.
 *
 * @return A string containing the merged parts, delimited if necessary.
 */
inline std::string MergeStrings(const std::string& group,
                                 const std::string& member,
                                 char delimiter = '.') {
  return member.empty() ? group : group + delimiter + member;
}
} // namespace utils

/**
 * A library for managing objects.
 *
 * The `Library` class provides functionality to add, load, retrieve, and check
 * for the existence of objects within the library. Each object is associated with
 * a unique name.
 *
 * @tparam ObjectType The type of object to be managed by the library.
 * @tparam Level The depth of the dictionary.
 */
template<typename ObjectType, size_t Level = 1>
class Library;

/// @brief Specialization of the `Library` class for 1 level of depth.
template<typename ObjectType>
class Library<ObjectType, 1>
{
public:
    // Constructor/Destructor
    // ----------------------------------------
    /// @brief Create a new library.
    Library(const std::string& name = "Object") : m_Type(name) {}
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
            CORE_WARN("{0} already exists!", GetTypeName());
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
    const ObjectType& Get(const std::string& name) const
    {
        bool exists = Exists(name);
        if (!exists)
            CORE_WARN("{0} not found!", GetTypeName());
        
        return m_Objects.at(name);
    }
    /// @brief Retrieves an object from the library by its name.
    /// @param name The name of the object to retrieve.
    /// @return The retrieved object.
    /// @note If the object with the specified name does not exist in the library, an assertion
    /// failure will occur.
    ObjectType& Get(const std::string& name)
    {
        bool exists = Exists(name);
        if (!exists)
            CORE_WARN("{0} not found!", GetTypeName());
        
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
            CORE_WARN("{0} not found!", GetTypeName());
    }
    /// @brief Checks if an object with a given name exists in the library.
    /// @param name The name of the object to check for existence.
    /// @return True if an object with the specified name exists in the library, otherwise false.
    bool Exists(const std::string& name) const
    {
        return m_Objects.find(name) != m_Objects.end();
    }
    
    /// @brief Checks if the library is empty.
    /// @return `true` if the library contains no elements, `false` otherwise.
    bool IsEmpty() const { return m_Objects.empty(); }
    
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
    const std::string& GetTypeName() const { return m_Type; }
    
    // Library variables
    // ----------------------------------------
protected:
    ///< A map of object names to their corresponding objects.
    std::unordered_map<std::string, ObjectType> m_Objects;
    ///< The name of the type of objects contained in the library.
    std::string m_Type;
};

/// @brief Specialization of the `Library` class for 2 level.
template<typename ObjectType>
class Library<ObjectType, 2>
{
public:
    // Constructor/Destructor
    // ----------------------------------------
    /// @brief Create a new library.
    Library(const std::string& type = "Object") : m_Type(type) {}
    /// @brief Delete the library.
    virtual ~Library() = default;

    // Add
    // ----------------------------------------
    /// @brief Adds an object to the library.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @param object The object to add.
    /// @note If an object with the same group and object names already exists, an assertion failure
    /// will occur.
    virtual void Add(const std::string& group,
                     const std::string& member,
                     const ObjectType& object)
    {
        std::string message = GetType() + " '" +
            utils::MergeStrings(group, member) + "' already exists!";
        CORE_ASSERT(!Exists(group, member), message);

        m_Objects[group].Add(member, object);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves an object from the library by its group and object names.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return The retrieved object.
    /// @note If the object does not exist, an assertion failure will occur.
    ObjectType& Get(const std::string& group,
                    const std::string& member)
    {
        std::string message = GetType() + " '" +
            utils::MergeStrings(group, member) + "' not found!";
        CORE_ASSERT(Exists(group, member), message);
        
        return m_Objects[group].Get(member);
    }

    /// @brief Retrieves an object from the library by its group and object names (const version).
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return The retrieved object.
    /// @note If the object does not exist, an assertion failure will occur.
    const ObjectType& Get(const std::string& group,
                          const std::string& member) const
    {
        std::string message = GetType() + " '" +
            utils::MergeStrings(group, member) + "' not found!";
        CORE_ASSERT(Exists(group, member), message);
        return m_Objects.at(group).Get(member);
    }
    
    /// @brief Checks if an object with given group and object names exists in the library.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return True if the object exists, otherwise false.
    bool Exists(const std::string& group,
                const std::string& member) const
    {
        auto groupIt = m_Objects.find(group);
        if (groupIt == m_Objects.end())
            return false;
        return groupIt->second.Exists(member);
    }
    
    // Iteration support
    // ----------------------------------------
    /// @brief Get the begin iterator for the library.
    /// @return Iterator pointing to the begin of the library.
    typename std::unordered_map<std::string, Library<ObjectType, 1>>::iterator begin()
    {
        return m_Objects.begin();
    }
    /// @brief Get the end iterator for the library.
    /// @return Iterator pointing to the end of the library.
    typename std::unordered_map<std::string, Library<ObjectType, 1>>::iterator end()
    {
        return m_Objects.end();
    }
    /// @brief Get the begin iterator for the library (constant value).
    /// @return Iterator pointing to the begin of the library.
    typename std::unordered_map<std::string, Library<ObjectType, 1>>::const_iterator begin() const
    {
        return m_Objects.begin();
    }
    /// @brief Get the end iterator for the library (constant value).
    /// @return Iterator pointing to the end of the library.
    typename std::unordered_map<std::string, Library<ObjectType, 1>>::const_iterator end() const
    {
        return m_Objects.end();
    }
    
protected:
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the name of the libraries that are contained in the library.
    /// @return The name of the libraries.
    const std::string& GetType() const { return m_Type; }

    // Library variables
    // ----------------------------------------
protected:
    ///< A map of group names to a map of object names and their corresponding objects.
    std::unordered_map<std::string, Library<ObjectType, 1>> m_Objects;
    ///< The name of the libraries contained in the library.
    std::string m_Type;
};
