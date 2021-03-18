#pragma once
#include <Arduino.h>

#include "Cmd.h"
#include "Global.h"

class Scenario {
   public:
    void loop() {
        if (!jsonReadBool(configSetupJson, "scen")) {
            return;
        }
        String allBlocks = scenario;
        allBlocks.replace("\r\n", "\n");
        allBlocks.replace("\r", "\n");
        allBlocks += "\n";

        String incommingEvent = selectToMarker(eventBuf, ",");
        String incommingEventKey = selectToMarker(incommingEvent, " ");
        String incommingEventValue = selectToMarkerLast(incommingEvent, " ");

        while (allBlocks.length() > 1) {
            String oneBlock = selectToMarker(allBlocks, "end\n");
            String condition = selectToMarker(oneBlock, "\n");
            if (condition.indexOf("&&") != -1) {  //если двойное условие
                String condition1 = selectFromMarkerToMarker(condition, "&&", 0);
                String condition2 = selectFromMarkerToMarker(condition, "&&", 1);
                if (isScenarioNeedToDoJson(condition1) && isScenarioNeedToDoJson(condition2)) {
                    oneBlock = deleteBeforeDelimiter(oneBlock, "\n");
                    oneBlock.replace("end", "");
                    SerialPrint("I", "Scenario", condition1 + " && " + condition2 + " \n" + oneBlock);
                    spaceCmdExecute(oneBlock);
                }
            } else if (condition.indexOf("+-") != -1) {  //если гистерезис
                if (isScenarioNeedToDo(condition, incommingEventKey, incommingEventValue, 2)) {
                    oneBlock = deleteBeforeDelimiter(oneBlock, "\n");
                    oneBlock.replace("end", "");
                    SerialPrint("I", "Scenario", condition + " \n" + oneBlock);
                    spaceCmdExecute(oneBlock);
                }
            } else {  //остальные случаи
                if (isScenarioNeedToDo(condition, incommingEventKey, incommingEventValue, 1)) {
                    oneBlock = deleteBeforeDelimiter(oneBlock, "\n");
                    oneBlock.replace("end", "");
                    SerialPrint("I", "Scenario", condition + " \n" + oneBlock);
                    spaceCmdExecute(oneBlock);
                }
            }
            allBlocks = deleteBeforeDelimiter(allBlocks, "end\n");
            eventBuf = deleteBeforeDelimiter(eventBuf, ",");
        }
    }

    bool isScenarioNeedToDo(String &condition, String &incommingEventKey, String &incommingEventValue, int type) {
        bool res = false;
        String setEventKey = selectFromMarkerToMarker(condition, " ", 0);
        if (isEventExist(incommingEventKey, setEventKey)) {
            String setEventSign;
            String setEventValue;
            if (type == 1) preCalculation(condition, setEventSign, setEventValue);
            if (type == 2) preCalculationGisteresis(condition, setEventSign, setEventValue);
            if (isConditionMatch(setEventSign, incommingEventValue, setEventValue)) {
                res = true;
            }
        }
        return res;
    }

    bool isScenarioNeedToDoJson(String &condition) {
        bool res = false;
        String setEventKey = selectFromMarkerToMarker(condition, " ", 0);
        String setEventSign;
        String setEventValue;
        preCalculation(condition, setEventSign, setEventValue);
        String jsonValue = getValue(setEventKey);
        if (isConditionMatch(setEventSign, jsonValue, setEventValue)) {
            res = true;
        }
        return res;
    }

    bool isScenarioNeedToDoJson2(String &condition, String &incommingEventKey, String &incommingEventValue) {
        bool res = false;
        String setEventKey = selectFromMarkerToMarker(condition, " ", 0);
        if (isEventExist(incommingEventKey, setEventKey)) {
            String setEventSign;
            String setEventValue;
            preCalculation(condition, setEventSign, setEventValue);
            if (isConditionMatch(setEventSign, incommingEventValue, setEventValue)) {
                res = true;
            }
        }
        return res;
    }

    void preCalculation(String &condition, String &setEventSign, String &setEventValue) {
        setEventSign = selectFromMarkerToMarker(condition, " ", 1);
        setEventValue = selectFromMarkerToMarker(condition, " ", 2);
        if (!isDigitDotCommaStr(setEventValue)) {
            setEventValue = getValue(setEventValue);
        }
    }

    void preCalculationGisteresis(String &condition, String &setEventSign, String &setEventValue) {
        setEventSign = selectFromMarkerToMarker(condition, " ", 1);
        setEventValue = selectFromMarkerToMarker(condition, " ", 2);
        if (!isDigitDotCommaStr(setEventValue)) {
            String setEventValueName = selectToMarker(setEventValue, "+-");
            String gisteresisValue = selectToMarkerLast(setEventValue, "+-");
            gisteresisValue.replace("+-", "");
            String value = getValue(setEventValueName);
            String upValue = String(value.toFloat() + gisteresisValue.toFloat());
            String lowValue = String(value.toFloat() - gisteresisValue.toFloat());
            if (setEventSign == ">") {
                setEventValue = upValue;
            } else if (setEventSign == "<") {
                setEventValue = lowValue;
            }
        }
    }

    bool isEventExist(String &incommingEventKey, String &setEventKey) {
        bool res = false;
        if (incommingEventKey == setEventKey) {
            res = true;
        }
        return res;
    }

    bool isConditionMatch(String &setEventSign, String &incommingEventValue, String &setEventValue) {
        boolean flag = false;
        if (setEventSign == "=") {
            flag = incommingEventValue == setEventValue;
        } else if (setEventSign == "!=") {
            flag = incommingEventValue != setEventValue;
        } else if (setEventSign == "<") {
            flag = incommingEventValue.toFloat() < setEventValue.toFloat();
        } else if (setEventSign == ">") {
            flag = incommingEventValue.toFloat() > setEventValue.toFloat();
        } else if (setEventSign == ">=") {
            flag = incommingEventValue.toFloat() >= setEventValue.toFloat();
        } else if (setEventSign == "<=") {
            flag = incommingEventValue.toFloat() <= setEventValue.toFloat();
        }
        return flag;
    }
};

extern Scenario *myScenario;
extern void streamEventUDP(String event);