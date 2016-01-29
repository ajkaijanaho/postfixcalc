# Copyright © 2016 Antti-Juhani Kaijanaho
#
# Licensed under the Apache License, Version 2.0 (the "License"); you
# may not use this file except in compliance with the License.  You
# may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.  See the License for the specific language governing
# permissions and limitations under the License.

CC = gcc --std=c99
CFLAGS = -Wall -Wextra -g -O2

# Call make with READLINE=1 to compile with readline support
ifeq '$(READLINE)' '1'
CFLAGS += -DWITH_READLINE
LDLIBS = -lreadline
endif

all : postfixcalc

clean :
	$(RM) postfixcalc *.o
