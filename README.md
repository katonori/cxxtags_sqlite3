cxxtags
=======

What is this?
------------------------
**cxxtags** is a tool to tag(index) C/C++ source files based on clang. The major difference from ctags is
C++ syntax(ex. class, namespace, template, etc.) support.

Several IDEs(Visual Studio, Eclipse, Xcode, etc.) already have this tagging(indexing) feature though, those are tightly
built-in and not portable(even though Eclipse(CDT) is a open source project). **cxxtags** aims to be light-weight and
portable source code tagging system.

### Requirement
**cxxtags** is written in C/C++ and python and based on clang indexer and sqlite3. You need stuffs listed
below to build and run this command.

* clang libs and headers(ver >= 3.2)
* sqlite3 library and C/C++ headers.
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
    * copy the contents of ${CXXTAGS_REPOSITORY_ROOT}/bin directory to your installation path

Commands
------------------------
**CAUTION**: Database format and the specification of these commands are drastically changed from previous version.

### cxxtags
Generates a database file.

    usage: cxxtags [-e exclude_list] [-o output_file] input_file [compiler_arguments]

* -e: Specify directories that contains source/header files that you don't want to tag. Directories are separated by ':' like "/usr/include:/usr/local/include".
* -o: Specify output file name. If no output file names are specified, the output file is named with suffix ".db" is used as output file name.

Compiler arguments are passed to clang. To get precise information you need to pass arguments just like you compile the source.

If some input files are or a input file named like "*.o" is passed to **cxxtags**, **cxxtags** pretend to be a linker command, 
outputs an empty file. This behavior is to be compatible with compiler commands such as g++ and clang.

### cxxtags_merger
Merges some database files generated by **cxxtags** to a directory. 

    usage: cxxtags_merger output_directory input_files [...]

This command updates database directory instead of replacing it if you specify database directory that already exists as *output_directory*.

For performance reason, this command delete input files. If you need input files to be remained, back it up by yourself.

### cxxtags_html_dumper

    usage: cxxtags_html_dumper database_directory

Convert tag database to html files just like htags in GNU global. This is sample tool for using tag database.
**cxxtags_html_dumper** takes the only one argument the tag database directory. This directory should be 
generated by **cxxtags_merger**.
**cxxtags_html_dumper** extract several information from the database and generate html files. 

### cxxtags_query
Make a query to the database.

    usage: cxxtags_query query_type[decl/def/ref/override/overriden] database_dir item_name file_name line_no column_no'
         : cxxtags_query query_type[name]                            database_dir item_name [-f file_name] [-p, --partial]'

* query_type: Specify the type of query. Listed below are acceptable.  
    * decl: Query information about location where the item is **declared**.  
    * def: Query information about location where the item is **defined**.  
    * ref: Query information about all the locations where the item is **refered**  
    * override: Query information about items that *item_name* overrides.
    * overriden: Query information about items that is overriden by *item_name*.
    * name: Search information about all items named *item_name* in database.
* database_dir: Database directory generated by **cxxtags_merger** command.  
* item_name: Specify the name of the item that you want to inspect.  
* line_no: line number.
* column_no: column number.
* options
    * in *name* mode
        * -f: Specify the file that contains the item.  
        * -p, --partial: Enable partial match.

How to use
------------------------
As mentioned before, **cxxtags** needs compile options such as "-I" "-D" to get correct information.
**cxxtags** is designed to be passed same arguments as you compile the source file by compiler such as g++
and clang. So the only thing you need to do is replace compiler command with **cxxtags**. if you compile the
source file like below,

    $ g++ -I/your/headers -DYOUR_MACRO -c -o a.o a.cpp

you can generate tag database by command below. The tag database "a.o" will be generated.

    $ cxxtags -e /usr/include -I/your/headers -DYOUR_MACRO -c -o a.o a.cpp

Like compiler commands, **cxxtags** doesn't depend on a tagging process of other files. So you can tag several
source files in parallel.

If you can add the rule to Makefile of your project, you may easily added the rule to generate
tag database like above. But if you want to tag a source package that you don't know match about, it may be
difficult to tag the source package. 

Some source packages can tag source codes by changing PATH search order like below.

* step 1: configure source package as usual.

        $ ./configure --prefix=/your/install/path

* step 2: make pseudo compiler commands
    * edit /path/to/cxxtags/installed/g++ as shown below 

            #!/bin/sh  
            cxxtags -e /usr $*  

    * set paermission and make aliases if needed. 

            $ chmod +x /path/to/cxxtags/installed/g++
            $ ln -s /path/to/cxxtags/installed/g++ /path/to/cxxtags/installed/gcc

* step 3: then start building
    
        $ env PATH=/path/to/cxxtags/installed:${PATH} make

* step 4: finaly, merge database files

        $ cxxtags_merger db `find ./ -name "*.o"`

* step 5: dump or query

        $ cxxtags_html_dumper db

    or

        $ cxxtags_query name db main

To index LLVM source package, you may need to build as usual before step 3 to build tools used in build process such as "tblgen".
Once you build as usual, then delete *only* obj files(except commands) by this command
    
    $ find . -name "*.o" -exec rm -fv {} \;

and then resume from step 3.

Example
------------------------
* You can find some examples at _test_ directory. You can generate tag database and html files by these commands.  

        $ cd ${CXXTAGS_REPOSITORY_ROOT}/test/ns/
        $ make html

  then a directory _${CXXTAGS_REPOSITORY_ROOT}/test/ns/html_ is generated. 

