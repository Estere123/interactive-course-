import { Session, Chatbox } from "@talkjs/react";
import './ChatBox.css';

export default function Chat() {
  return (
    <div className="chat-container">
      <Session appId="t78P9qHj" userId="sample_user_alice">
        <div className="ChatBox">
          <Chatbox conversationId="sample_conversation" />
        </div>
      </Session>
    </div>
  );
}