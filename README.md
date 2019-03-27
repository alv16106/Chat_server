# Chat_server
## Protocolo
Status = (0: active, 1: busy, 2: inactive)
```javascript
user:{
 id: number,
 username: string,
 status: 0,
 last_connected: time,
}
```

### Request connection
```javascript
{
 code: 0,
 data:{
   username: string,
 }
}
```

#### SUCCESS
```javascript
{
 code: 200,
 data: {
   user:{
     id: number,
     username: string,
     status: 0,
   }
 }
}
```

#### ERROR
```javascript
{
 code: 500,
 data: {
   error_message: string,
 }
}
```

### Send message
```javascript
{
 code: 1,
 data: {
   to: List<id>,
   message: string,
   file_type:? string,
 }
}
```

### Receive message
```javascript
{
 code: 201,
 data: {
   from: username,
   message: string,
 }
}
```

### GET user
```javascript
{
 code: 3,
 data:{
   user: List<id>,
 }
}
```

### Recieve user
```javascript
{
 code: 203,
 data: {
   users: List<Users>,
 }
}
```

### Change status
```javascript
{
 code: 4,
 data: {
   user: id,
   new_status: number,
 }
}
```

### Change status success
```javascript
{
 code: 204,
 data: {},
}
```
