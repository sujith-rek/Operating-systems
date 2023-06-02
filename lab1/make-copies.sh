#!/bin/bash
i=-1
while (( i++ < 5000 ));
do
  cp ./foo.pdf "./diskfiles/foo$i.pdf"
done
