#Interfaces in Json 
##1	begin_to_ game
###1.1	Request Description
####1.1.1	Precondition & Background: 
When a user opens the game, CLIENT should compose this message and send it first. In this game, ��login�� is not necessary. CLIENT can give a user name and corresponding uuid instead of ��login��.
####1.1.2	Fields 
[[[
||*req*: ||  begin_to_game ||

||*user_name*: ||  A random string is generated by CLIENT as user name . ||

||*uuid*: || Universally Unique Identifier, it is also generated by CLIENT. ||
]]]
###1.2	Response Description
####1.2.1	Precondition & Background
After receiving *begin_to_game* request, SERVER will query a user list to check whether the uuid had already existed. If it does, SERVER can find its score and other information to reply to CLIENT. Otherwise, Server can give a note indicates it is a new user in this game. 
In addition, SERVER will choose a map in game.
####1.2.2	Fields 
rep:  begin_to_game

result:  success or failure will be filled in this field. It shows the request is success or not.

failure_note:  if the result is failure, this field will contain a statement to explain what reason to the failure is. Otherwise, this filed should be null.

info:  it is a sub directory containing score and other information for the user. If the uuid doesn��t exist, this filed will be null.
      max_score:  a key in the info, it is the max score the user played before.
      ava_score:  a key in the info, it is the average score the user played before. 
      level:  a key in the info, it represents the users playing level.

      map:  A index number is here, indicating which map is chosen in user��s game.   
##2	begin to match
###2.1	Request Description
####2.1.1	Precondition & Background 
If a user intends to play with other users via internet, this message should send first. Some network information should be included in this message in order to establish a connection between two players.
####2.1.2	Fields 
req:  begin_to_match

uuid:  Universally Unique Identifier, it is also generated by CLIENT. It should be same as ��begin_to_game��.

level:  it represents the user��s playing level. The value is got from ��begin_to_game�� response message.

ip:  user��s ip address. 

port: user��s port number. 

2.2	Response Description
2.2.1	Precondition & Background
This message should reply according to the request. SERVER will add the user in request to ��match_user_list��. And SERVER will find a suited opponent from ��match_user_list�� to reply.

2.2.2	Fields 
rep:  --begin_to_match--

result:  ��success�� or ��failure�� will be filled in this field. It shows the request is success or not.

failure_note:  if the ��result�� is ��failure��, this field will contain a statement to explain what reason to the failure is. Otherwise, this filed should be null.

opponent:  it is a sub-directory, storing opponent��s information.
           uuid: opponent��s uuid. 
           ip:  opponent��s ip addres.
           port:  opponent��s port number. 
           level:  opponent��s level.

3	step 
3.1	Request Description
3.1.1	Precondition and Background
This interface will gather the current step and current status of chessboard. These information can be applied to generate history and judge victory or not.

3.1.2	Fields
req:  step

uuid:   Universally Unique Identifier, it is also generated by CLIENT. Its value should be same as ��begin_to_game��

direct_vector:  it an array, having 2 elements. The first represents increment of x axis, the second is for y axis. So all the directories can be shown as [1,0], [-1,0], [0, 1], [0, -1], [1, -1], [1, 1], [-1, 1] and [-1, -1].

3.2	Response Description
3.2.1	Precondition and Background
SERVER will store current step of the user and judge whether the user is victory. If the user is victory, SERVER will record the score and adjust level of the user.
3.2.2	Fields
rep:  step

result:  ��success�� or ��failure�� will be filled in this field. It shows the request is success or not.

failure_note:  if the ��result�� is ��failure��, this field will contain a statement to explain what reason to the failure is. Otherwise, this filed should be null.

victory:  if the user is victory after this step,  ��true�� will be filled in this field. Otherwise, ��false�� is filled instead.

4	end_game
4.1	Request Description
4.1.1	Precondition and Background
When a game is ended by user or the winner is determined, this message should be sent to SERVER.
4.1.2	Fields
req:   end_game

uuid:  Universally Unique Identifier, it is also generated by CLIENT. Its value should be same as ��begin_to_game��.
4.2	Response Description
4.2.1	Precondition and Background
SERVER will end the game and reply to CLIENT.
4.2.2	Fields
rep:  end_game

result:  ��success�� or ��failure�� will be filled in this field. It shows the request is success or not.

failure_note:  if the ��result�� is ��failure��, this field will contain a statement to explain what reason to the failure is. Otherwise, this filed should be null.


