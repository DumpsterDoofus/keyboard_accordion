import random
import tkinter as tk
from PIL import ImageTk, Image
from music21 import *
import mido

from music21 import *
from music21 import environment
import os
env = environment.Environment()

env['musescoreDirectPNGPath'] = '/Applications/MuseScore 4.app/Contents/MacOS/mscore' 

# --- CONFIGURATION ---
MIDI_MIN = 57 - 12 * 3
MIDI_MAX = 58 + 12 * 3
IMAGE_DIR = "temp"

class NoteTrainer:
    def __init__(self, root):
        self.root = root
        self.root.title("MIDI Note Trainer")
        
        # 1. Ask user to select MIDI port first
        self.inport = None
        self.select_midi_device()
        
        # If no port was selected (user closed window), exit
        if not self.inport:
            self.root.destroy()
            return

        # UI Elements (Same as before)
        self.label = tk.Label(root, text=f"Connected to: {self.inport.name}")
        self.label.pack(pady=5)
        
        self.image_label = tk.Label(root)
        self.image_label.pack(pady=20)
        
        self.status_label = tk.Label(root, text="", fg="red")
        self.status_label.pack(pady=10)

        self.current_note = None
        self.new_note()

    def select_midi_device(self):
        """Creates a popup window to choose from available MIDI inputs."""
        input_names = mido.get_input_names()
        
        if not input_names:
            messagebox.showerror("Error", "No MIDI devices detected!")
            return

        # Create a popup window
        top = tk.Toplevel(self.root)
        top.title("Select MIDI Input")
        top.geometry("300x300")
        top.grab_set()  # Make this window modal

        tk.Label(top, text="Select your Accordion/Keyboard:").pack(pady=10)
        
        listbox = tk.Listbox(top, width=40)
        listbox.pack(padx=10, pady=5)
        for name in input_names:
            listbox.insert(tk.END, name)

        def on_select():
            selection = listbox.curselection()
            if selection:
                port_name = listbox.get(selection[0])
                self.inport = mido.open_input(port_name, callback=self.midi_callback)
                top.destroy()

        tk.Button(top, text="Connect", command=on_select).pack(pady=10)
        
        # Wait for the user to finish with this window before continuing
        self.root.wait_window(top)

    def new_note(self):
        """Selects a random note (different from the last) and updates the GUI."""
        # 1. Store the previous note
        old_note = self.current_note
        
        # 2. Keep picking until the note is different
        new_selection = random.randint(MIDI_MIN, MIDI_MAX)
        while new_selection == old_note:
            new_selection = random.randint(MIDI_MIN, MIDI_MAX)

        self.current_note = new_selection

        img_path = f"{IMAGE_DIR}/{self.current_note}-1.png"
        if not os.path.exists(img_path):
            n = note.Note(self.current_note)
            n.duration.type = 'half'
            
            # # 2. Create a Stream and add a Treble Clef
            # s = stream.Stream()
            # s.append(clef.TrebleClef()) 
            # s.append(n)
            
            n.write('musicxml.png', fp=f'images/{self.current_note}.png')        # Load and display the image
        if not os.path.exists(img_path):
            raise ValueError('Failed to create ' + img_path)
        img = Image.open(img_path)
        # Optional: Resize image if MuseScore renders it too large
        # img = img.resize((300, 200)) 
        self.photo = ImageTk.PhotoImage(img)
        self.image_label.config(image=self.photo)
        self.status_label.config(text="", fg="black")

    def midi_callback(self, message):
        """This runs every time a MIDI key is pressed."""
        if message.type == 'note_on' and message.velocity > 0:
            if message.note == self.current_note:
                # Use .after() to interact with Tkinter from a thread
                self.root.after(0, self.new_note)
            else:
                error_msg = f"Wrong! Played {message.note}, expected {self.current_note}"
                self.root.after(0, lambda: self.status_label.config(text=error_msg, fg="red"))

# Start the App
if __name__ == "__main__":
    root = tk.Tk()
    app = NoteTrainer(root)
    root.mainloop()
