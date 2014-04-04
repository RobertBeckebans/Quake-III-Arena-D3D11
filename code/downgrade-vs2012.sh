#!/bin/bash

for proj in $(find . -name '*VS2013.vcxproj'); do

# get the new filename
	newproj=$(echo $proj | sed -e "s/VS2013/VS2012/g")
	echo Downgrading $proj ... $newproj

# copy the contents
	cp -f $proj $newproj

# copy the filters file
	cp -f $proj.filters $newproj.filters

# remove references to the 2013-only FxCompile option
	sed -i -e "s/FxCompile/None/g" $newproj.filters

# replace any mention of other projects with the new project
	sed -i -e "s/VS2013.vcxproj/VS2012.vcxproj/g" $newproj

# upgrade the toolset version
	sed -i -e "s/ToolsVersion=\"12.0\"/ToolsVersion=\"4.0\"/g" $newproj

# upgrade the platform toolset version
	sed -i -e "s/<PlatformToolset>v120<\/PlatformToolset>/<PlatformToolset>v110<\/PlatformToolset>/g" $newproj

done


# upgrade the solution file
for sln in $(find . -name '*VS2013.sln'); do
	newsln=$(echo $sln | sed -e "s/VS2013/VS2012/g")
	echo Downgrading $sln ... $newsln

	cp -f $sln $newsln

	# replace any mention of 2013 with 2012
	sed -i -e "s/2013/2012/g" $newsln
done




