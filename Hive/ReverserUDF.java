package com.req4;

import org.apache.hadoop.hive.ql.exec.Description;
import org.apache.hadoop.hive.ql.exec.UDF;

public class ReverserUDF extends UDF {
    public String evaluate(String str){
        return new StringBuilder(str).reverse().toString();
    }

}
