cxxtags
=======

What is this?
------------------------
**cxxtags** is a tool to tag(index) C/C++ source files based on clang. The major difference from ctags is
C++ support. C++ syntax (ex. class, namespace, template, etc.) is supported(not fully yet though). And
generated tag file is a sqlite3 database file. So you can access tag information via sqlite3 queries.

Several IDEs(Visual Studio, Eclipse, Xcode, etc.) have this feature though, it is tightly built-in and not
portable(even though Eclipse(CDT) is a open source project). **cxxtags** aims to be light-weight and portable
source code tagging system.

### Requirement
**cxxtags** is written in C/C++ and python and based on clang indexer and sqlite3. You need stuffs listed
below to run/build this command.

* clang libs and headers
* python with sqlite3 support(ver >=2.5)

**cxxtags** is developped and tested on Mac OS X 10.8.2, python-2.7.2 and clang(LLVM)-3.2.
But it is expected to be able to run on other Unix-like systems.

### How to build
* check out repository

        $ git clone https://github.com/katonori/cxxtags.git

* specify path LLVM is installed.
 * set LLVM_HOME in _${CXXTAGS_REPOSITORY_ROOT}_/src/Makefile correctly.

* build

        $ cd ${CXXTAGS_REPOSITORY_ROOT}/src && make

* install
 * copy **cxxtags**, **cxxtags_core**, **cxxtags_merger** and **cxxtags_html_dumper** to your installation path

Commands
------------------------
### cxxtags
generates a database file.

    usage: cxxtags [-e exception_list] [-o output_file] input_file [compiler_arguments]

* -e: Pass list of directory that contains sources that you don't want to tag. Directories are separated by ':' like "/usr/include:/usr/local/include".
* -o: Specify output file name. If no output file names is specified, the input file name with suffix ".db" is used as output file name.

Compiler arguments are passed to clang. To get precise information you need to pass arguments just like you
compile the source.

### cxxtags_merger
merges some database files generated by **cxxtags** to a database file. The database format is the same as
the file generated by **cxxtags**.

    usage: cxxtags_merger output_file input_files [...]

if the duplicated row entries are found, the only one row entry will remain in output database.

### cxxtags_html_dumper

    usage: cxxtags_html_dumper database_file

Convert tag database to html files just like htags in GNU global. This is sample tool for using tag database.
**cxxtags_html_dumper** takes the only one argument the tag database file name. **cxxtags_html_dumper**
extract several information from the database and generate html files. If you want to generate html file
about several source files, you need to generate tag database of each source file using **cxxtags** and then
merge those database files using **cxxtags_merger**.

How to use
------------------------
As mentioned before, **cxxtags** needs compile options such as "-I" "-D" to get correct information.
**cxxtags** is designed to be passed same arguments as you compile the source file by compiler such as g++
and clang. So the only thing you need to do is replace compiler command with **cxxtags**. if you compile the
source file like below,

    g++ -I/your/headers -DYOUR_MACRO -c -o a.o a.cpp

you can generate tag database by command below. The tag database "a.o" will be generated.

    cxxtags -e /usr/include -I/your/headers -DYOUR_MACRO -c -o a.o a.cpp

Like compiler commands, **cxxtags** doesn't depend on tagging process of other files. So you can tag several
source files in parallel.

If you can add the rule to Makefile of your target source package, you may easily added the rule to generate
tag database like above. But if you want to tag a source package that you don't know match about, it may be
difficult to tag the source package. Using build log the source package may be good idea.

Once you created a database you can access the database via sqlite3 queries. The simplest way to access
database is to use sqlite3 command. For example,

    $ sqlite3 your_database "SELECT * FROM ref"

shows you all the contents in table "ref". The table definition is described in
[Table Definition](https://github.com/katonori/cxxtags/wiki/Table-definition "Table Definition").

Example
------------------------
You can find some examples at _test_ directory. You can generate tag database and html files by these commands.  

    $ cd ${CXXTAGS_REPOSITORY_ROOT}/test/ns/
    $ make html

then a directory _${CXXTAGS_REPOSITORY_ROOT}/test/ns/html_ is generated. 

And you can query information to tag database like below.  

    $ sqlite3 index.db "SELECT file_name, line, col FROM DECL WHERE name=\"main\""
    /Users/katonori/cxxtags/test/ns/main.cpp|7|5

This query statement queries a file name, line number and column number of a node named _main_.
And result shows a node named _main_ found at line 7, column 5 of the file _/Users/katonori/cxxtags/test/ns/main.cpp.

Known problems
------------------------
* processing speed  
 cxxtags project is still in early development stage so processing speed of generating tags database is very
slow. Especially, database merging process by **cxxtags_merger** is slow. If you aim to tagging large source
package, you need to select the part you are interested the best. Improvement of the speed is a future work. 
