#ifndef ARG_PARSER_H
#define ARG_PARSER_H

//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
//
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include <vector>
#include <iostream>

/** \brief A utility class to ease the parsing of command-line
    arguments

    The ArgParser class provides the necessary features to easily
    parse user-specified command-line options and set the appropriate
    variables for use in the rest of the program.
	
    \code

    int main(int argc, char* argv[]) {
    
    bool   arg1 = true;      // These must be visible to the arg_list array.
    bool   arg2 = false;
    string arg3 = "";
    int    val = -1;
    ArgParser::ArgRecord arg_list[] = {
        {"--arg1", "-1", "Toggle arg1", &arg1, ArgParser::BOOLEAN}, 
        {"--arg2", "-2", "Toggle arg2", &arg2, ArgParser::INV_BOOLEAN},
        {"--arg3", "-3", "Set arg3 to the specified string", &arg3,
         ArgParser::STRING},
        {"--arg4", "-4", "Set value to use", &val, ArgParser::INTEGER},
        {"", "", NULL, ArgParser::INVALID}
    };

    // Create arg parser and process command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv);

    // Validate if necessary parameters have been specified.
    bool success = ap.checkArg((val > 0), &val) &&
                   ap.checkArg(!arg3.empty(), &arg3, "arg3 must be specified");
    if (success) {
       // The arguments have now been parsed and ArgParser has set
       // them appropriately.
    }
    
    }  // main
    \endcode

*/
class ArgParser {
    /** Prints a nicely formatted list of parameters.  This method is
        used when printing help information.

        \param[out] os The output stream to where the data is to be
        written.

        \param[in] ap The argument parser whose parameters is to be
        printed in a nice format.

        \return The output stream passed in as the parameter.
    */    
    friend std::ostream& operator<<(std::ostream&, ArgParser&);

public:
    /** \brief Specify the argument type
        
        The values in this enumeration are used by ArgParser to
        identify what type of data the command-line option expects, so
        that it can appropriately parse and set the value.

	\note The ArgType::INFO_MESSAGE enumeration is not really a
	parameter. It can be used to insert an informational message
	into the argument list.  The help entry of this arg type is
	written to the screen.
    */
    enum ArgType {BOOLEAN, INTEGER, UNSIGNED_INT, STRING, STRING_LIST,
                  FLOAT, DOUBLE, LONG, INFO_MESSAGE, MAIN_MESSAGE,
                  INV_BOOLEAN, INVALID};

    /** \typedef typedef std::vector<std::string> StringList

	\brief Shortcut type definition to be used with ArgParser::STRING_LIST.

	This type definition provides a short cut for a vector of
	strings. This is the data type that must be used with the
	ArgParser::STRING_LIST enumeration in this class.  Here is
	brief example:

	\code

	StringList fileList
	ArgParser::ArgRecord arg_list[] = {
	    {"--inputFiles", "-if", "One or more input files",
             &fileList, ArgParser::STRING_LIST},
	    {"", "", "", NULL, ArgParser::INVALID}
	};

	\endcode
    */
    typedef std::vector<std::string> StringList;
	
    /** \brief Aggregate record to store the necessary information to
        parse arguments and store the values back for use.
        
        The arg_record is used to store four pieces of necessary
        information to parse the valid arguments, show a helpful
        message to the user, and place the parsed value back for the
        program to use.

    */
    struct ArgRecord {
        /**
           Store the command's name, usually prefixed by two hyphens
           ("--") in the GNU Long Option style.
        */
        std::string command;

        /**
           Store the short-form of this command, usually prefixed by a
           single hyphen ("-") in GNU short option style.
         */
        std::string shortCommand;
        
        /**
           A one-line description of the argument's function
        */
        std::string help;

        /**
           A pointer to the location of memory to store the specified
           value. This must be of the same type as specified by the
           type argument.
        */
        void* data;

        /**
           The type of the argument, which is used for both parsing of
           the value and storage back into the specified location.
        */
        ArgType type;
    };

    /** \brief Default constructor to create an empty, default
        ArgParser

        This constructor creates and initializes an empty ArgParser
        capable of understanding only "--help". Before it can be used
        properly, the valid arguments must be populated using
        addValidArguments().

	\param[in] info An optional informational message to be
	displayed to the user (before list of arguments) when help is
	requested.
    */    
    ArgParser(const std::string& info = "");

    /** \brief Recommended constructor for creating a usable
        ArgParser.

        This constructor accepts an array of valid arguments that can
        be parsed and prepares the structure for use internally.
        
        The functionality provided by this constructor is identical to
        useing the no-argument constructor and the
        loadValidArguments() method.
        
        \param[in] validArguments An array of arg_record terminated by
        a {NULL, NULL} entry to indicate the end of the array.

	\param[in] info An optional informational message to be
	displayed to the user (before list of arguments) when help is
	requested.
    */
    ArgParser(const ArgRecord validArguments[], const std::string& info = "");

    /** \brief Default destructor.

        The destructor is responsible for cleaning up memory allocated
        by the ArgParser. Currently, there ArgParser does not
        allocate any memory and therefore the destructor does nothing.

    */
    ~ArgParser() {};

    /** \brief Adds additional set of parsable arguments to this
        argument parser.

        This method is used to add new/additional set of parsable
        arguments. This methods grows the set of all of possible
        arguments that this parser is to be able to parse. The
        parameter also provides pointers to the memory locations that
        must be used to store the argument-values based on the results
        of the parsing.

        \note This method does not need to be called if the parser is
        created with a default set of arguments (using the
        ArgParser(const ArgRecord&) constructor).

        \param[in] validArguments An array of arg_record terminated by
        a {"", "", NULL, ArgType::INVALID} entry to indicate the end
        of the array.
    */
    void addValidArguments(const ArgRecord validArguments[]);
	
    /** \brief Parse the supplied arguments for the valid arguments
        and set the appropriate values

        This method is to be invoked with the argc and argv as
        provided by the operating system after the parser has been
        initialized with all valid arguments.

        The valid arguments list is then processed and matching
        arguments are parsed from the argument vector.  The data
        values in the records are suitably updated.  In addition,
        parsed arguments are then removed from argv and argc is
        appropriately updated.

        \param[in,out] argc The argument count. This value is upated
        to reflect the number of pending arguments in argv after valid
        arguments have been processed and removed.

        \param[in,out] argv The argument vector. Valid entries are
        removed from this list after they are processed.

        \param[in] caxoe Compain And eXit On Error. Defaults to \c
        true, set to \c false to suppress calls to exit().
    */
    void parseArguments(int& argc, char* argv[], bool caxoe = true);

    /** \brief Check the argument vector for any leftover, unparsed
	entries.

	This method is used to check the argument vector for any
	invalid, extra entries, and if necessary, raise an error
	message and terminate the program.

	\param[in] argc The argument count.

	\param[in] argv The argument vector.

	\param[in] caxoe Compain And eXit On Error. If this parameter
	is true and an argument (beginning with a \c '-') is found
	remaining then this method exits (after displaying an error
	message and printing help).  Otherwise it simply prints help
	and returns with \c false.

	\param[in] caxoa Complain And eXit on Argument.  If this
	parameter is \c true and more than 1 argument is left, then
	this method displays an error message, prints help, and returns
	\c false.

	\return This method returns \c true if the remaining arguments
	are valid. Otherwise it returns \c false to indicate that
	further processing should logically terminate.
    */
    bool checkRemainingArguments(int argc, char* argv[],
                                 bool caxoe = true, bool caxoa = false);

    /** Helper method to locate and change an argument record in
	argRecords.

        This is a helper method that is used by the various forms of
        setArgument method to locate a given argument, check its data
        type, and update its value.  This method is templatized so
        that it can handle the various value parameters elegantly.
        
        \return This method returns true if the specified arg was
        valid and its value was successfully updated.  Otherwise this
        method returns false.
        
        \param[in] argType The expected data type of the argument.

        \param[in] value The new value to be associated with the
        argument.
        
        \param[in] arg The argument whose value is to be changed.  The
	argument string must exactly match one of the entries in the
	ArgParser::argRecord vector.

    */    
    template <typename ValueType>
    bool setArgument(const std::string& arg, const ArgParser::ArgType argType,
		     const ValueType& value) {
        // First locate the appropriate entry in the argRecord
        for(size_t idx = 0; (idx < argRecords.size()); idx++) {
            ArgRecord& rec = argRecords[idx];
            if ((rec.command == arg) && (rec.type == argType)) {
                // Found a match
                *(reinterpret_cast<ValueType*>(rec.data)) = value;
                return true;
            }
        }
        // No matches found
        return false;	  
    }

    /** Convenience method to check condition on a given parameter.

        This is a convenience method that can be used to check if the
        value for a given parameter is setup correctly and if not
        report an error.  See top-level help for an example code on
        how to use this method.
        
        \param[in] condition Flag to indicate if the value for the
        specified data has been determined to be valid or invalid.
        Note that the check is performed by the caller and not by this
        method.  If this flag is true (indicating paramater setting is
        valid), this method does not perform any further information.
        
        \param[in] data The data member being checked.  The pointer
        must be the same pointer supplied as part of the argument list
        passed to the constructor.

        \param[in] message An optional message to be displayed to the
        user to provide additional information about the source of the
        issue.

        \param[out] os The output stream to where the error messages
        and help information is to be written.
        
        \return This method returns condition passed-in to this
        method.
     */
    bool checkArg(const bool condition, const void* data,
                  const std::string& message = "",
                  std::ostream& os = std::cerr) const;
    
                  
protected:
    /** This is a helper method that is used to print a single command
        line argument.

        This is a helper method that is called from operator<<() to
        print a single command line argument.

        \param[in] os The output stream to which the information about
        the argument is to be formatted and written.
        
        \param[in] indentStr A string containing blank spaces to be
        printed at the beginning to indent command line parameters to
        make them look nice.

        \param[in] numSpaces The number of spaces to be displayed
        between the argument and its help/description.

        \param[in] argRec The information about the argument to be
        displayed by this method.

        \param[in] maxWidth The maximum number of characters that a
        line can occupy.  The description of the command is suitably
        wrapped not to exceed this size.
    */
    static void printArg(std::ostream& os, const std::string& indentStr,
                         const int numSpaces, const ArgRecord& argRec,
                         const int maxWidth = 80);

    /** Helper method to get string representation of current value in
	a given arg record for printing.

	This method is a helper method that is called from printArg()
	method to obtain the string representation of the value
	specified for a given arg record.  This method utilizes the
	ArgType and current data value for the given argument record
	and approriately converts data to a string.

	\param[in] argRec The information about the argument whose
        value is to be returned as a string by this method.

	\return The string representation of the data value stored in
	this argument record.
    */
    static std::string getValue(const ArgRecord& argRec);

private:
    /** \brief Remove the specified argument from the argument vector,
        shifting the others appropriately.
        
        This method is a convenience method that merely calls
        memmove() to shift the contents of the argv array by one,
        effectively removing the specified element.
        
        \note This does not destroy the strings that argv points to;
        they continue to remain valid.

        \param[in] removeIdx The index of the argument to remove.
        
        \param[in,out] argc The argument count.

        \param[in,out] argv The argument vector.

    */
    void removeArgument(int removeIdx, int& argc, char* argv[]);
	
    /** \brief A pointer to the valid argument records.
        
        This variable stores list of valid arguments that can be
        parsed.  Entries are added via the addValidArguments() method
        in this class.
    */
    std::vector<ArgRecord> argRecords;

    /** Informational message to be displayed to the user.

	This string is simply displayed before the list of arguments
	are displayed to the user.  This value is set in the
	constructor(s) and is never changed during the life time of
	this clas.
    */
    std::string info;
};

#endif
