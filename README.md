wiringPi README
===============

Please note that the official way to get wiringPi is via git from
git.drogon.net and not GitHub.

ie.

  git clone git://git.drogon.net/wiringPi

   

The version of wiringPi held on GitHub by "Gadgetoid" is used to build the
wiringPython, Ruby, Perl, etc. wrappers for these other languages. This
version may lag the official Drogon release.  Pull requests may not be
accepted to Github....

Please see

  http://wiringpi.com/

for the official documentation, etc. and the best way to submit bug reports, etc.
is by sending an email to projects@drogon.net

Thanks!

  -Gordon


   同步  git://git.drogon.net/wiringPi 最新源代码到项目当中。
     此修改基于 原来的master 分支  

   `   git remote add upstream git://git.drogon.net/wiringPi.git  `

   `    git fetch upstream master:upstream/master  `

   `     git merge upstream/master     `

   `     git push origin master:master  `