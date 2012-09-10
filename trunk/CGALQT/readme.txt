set up your programming enviroment
================
*os: winxp sp3
*ide: visual studio 2005/2008 + sp1

*If you wanna program in other os or other compilers, ok, just quit this file, we don't support them now: )


get the source from our svn server
================
*get TortoiseSVN
*svn address: svn://10.214.54.14/
*get the user and psw from xu qiuer
*download the source code


get the third part lib
================
don't ask me what's it used for, just google it, you will get the better answer: )


1.boost
~~~~~~~~~~~~~~~
*unrar to anywhere you like, eg. c:\boost_1_37_0, and not contain the space, such as c:\program files\boost_1_37_0 and the below is the same.

*add "c:\boost_1_37_0" to "vs2005->tools->options->projects and solutions->vc++ directories->include files"
*it's so famous and useful that we could use it in every project we developed.

*add to windows system enviroment variable: BOOSTROOT = c:\boost_1_37_0


2.CGAL
~~~~~~~~~~~~~~~
*unrar to anywhere you like, eg. c:\cgal-3.4.1

*add to windows system enviroment variable: CGALROOT = c:\cgal-3.4.1



3.QT
~~~~~~~~~~~~~~~
*need patch the vs2005 KB930859 first if you use vs2005

*get the compilied the lib/dll from svn or
*another way is compiling it youself, how to? google. it takes a long time depending on your pc's hardware and commonly you can have a lunch or a cup of tea: )

*add to windows system enviroment variables: QMAKESPEC = win32-msvc2005 and QTDIR = d:\qt\4.2.3
*attach a new value to the path enviroment variable: path = d:\qt\4.2.3\bin;%path%

*attach a new value to the path enviroment variable: path = C:\Program Files\OpenCV\bin;%path%


other tools
===============
...

email&msn
===============
email\gtalk:fallsonxu@gmail.com
msn:fallson1981@hotmail.com
