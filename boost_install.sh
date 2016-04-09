if [ -d boost ]; then
  echo "-- Boost already installed"
  exit 0
fi

echo -n "-- Downloading Boost library"
curl --silent "http://netix.dl.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.tar.bz2" > boost_1_60_0.tar.bz2

RET=$?
if [ $RET -ne 0 ]; then
	echo " failed"
	exit $RET
fi

echo " done"

echo -n "-- Extracting Boost library"
tar -xf boost_1_60_0.tar.bz2

RET=$?
if [ $RET -ne 0 ]; then
  rm boost_1_60_0.tar.bz2
	echo " failed"
	exit $RET
fi

mv boost_1_60_0 boost

echo " done"

echo -n "-- Cleaning up environment"
rm boost_1_60_0.tar.bz2

RET=$?
if [ $RET -ne 0 ]; then
	echo " failed"
	exit $RET
fi

echo " done"

exit 0
