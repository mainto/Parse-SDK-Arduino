/*
 *  Copyright (c) 2015, Parse, LLC. All rights reserved.
 *
 *  You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
 *  copy, modify, and distribute this software in source code or binary form for use
 *  in connection with the web services and APIs provided by Parse.
 *
 *  As with any software that integrates with the Parse platform, your use of
 *  this software is subject to the Parse Terms of Service
 *  [https://www.parse.com/about/terms]. This copyright notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)

#include "../ParsePush.h"
#include "../ParseClient.h"
#include "../ParseUtils.h"

ParseLiveEvent::ParseLiveEvent(String response) {
	Serial.println(response);
	this->response = "";
	this->op = "";
	this->requestId = 0;
	this->errorCode = 0;
	
	int p = 0;
	if(response.charAt(p) != '{'){
		//fail
		Serial.println("Failed");
		return;
	}
	p++;
	
	String key = "";
	String value = "";
	bool isOpen = false;
	bool isValue = false;
	bool isObject = false;
	while(response.length() > p){
		char c = response.charAt(p++);
		if(isObject){
			switch(c){
				case ':': {
					break;
				}
				case '}': {
					this->response.concat(c);
					isObject = false;
					break;
				}
				case '{': {
					isOpen = true;
				}
				default: {
					this->response.concat(c);
				}
			}
		}
		else{
			switch(c){
				case '"': {
					break;
				}
				case '}':
				case ',': {
					//set value
					if(isValue) {
						//value of current key
						if(key == "op") {
							this->op = value;
						}
						else if(key == "requestId") {
						    this->requestId = value.toInt();
						}
						else if(key == "code") {
						    this->errorCode = value.toInt();
						}
					}
					isValue = false;
					key = "";
					value = "";
					break;
				}
				case ':': {
					isValue = true;
					if(key == "object") {
						//object
						isObject = true;
					}
					break;
				}
				default:{
					if(isValue) {
						value.concat(c);
					}
					else{
						key.concat(c);
					}
				}
			}
		}
	}
}

void ParseLiveEvent::close() {
}

#endif // ARDUINO_ARCH_ESP8266

