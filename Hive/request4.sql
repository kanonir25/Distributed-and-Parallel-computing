add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
USE osipinius;
add jar new/jar/Reverser.jar;
create temporary function reverser as 'com.req4.ReverserUDF';

SELECT ip, reverser(ip)
from user_logs;

