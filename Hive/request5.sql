add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.9.0.jar;

USE osipinius;

add FILE run.sh;

select TRANSFORM (request)
	USING 'run.sh'
	as (info)
from user_logs;
