
// This is valid for a single view

UWK.callbacks = {};

UWK.sendMessage = function(msgName, msgValue)
{
	msgValue = JSON.stringify(msgValue);
	UWK._sendMessage(UWK.viewId, msgName, msgValue);
};

UWK.subscribe = function(msgName, callback)
{
	var callbacks = UWK.callbacks[msgName];

	if (callbacks === undefined)
	{
		callbacks = UWK.callbacks[msgName] = [];
	}

	if (callbacks.indexOf(callback) != -1)
		return;

	callbacks.push(callback);
};

UWK.messageReceived.connect ( function(msgName, msgString)
{

	var callbacks = UWK.callbacks[msgName];

	if (callbacks === undefined)
		return;

	var msgObject = JSON.parse(msgString);

	for (var i = 0; i < callbacks.length; i++) 
	{
  		callbacks[i](msgName, msgObject);
	}	

});
