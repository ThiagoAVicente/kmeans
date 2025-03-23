import pandas as pd

# Step 1: Load the CSV file
df = pd.read_csv('tracks_features.csv')

# Step 2: Drop the unnecessary columns
# List the columns you want to keep or drop
columns_to_drop = ['album_id', 'artist_ids', 'disc_number', 'explicit','year','id','track_number','disc_number','release_date']  # Example, modify as needed

df_cleaned = df.drop(columns=columns_to_drop)

# Step 3: Save the cleaned DataFrame to a new CSV
df_cleaned.to_csv('cleaned_tracks_features.csv', index=False)

print("Unnecessary columns removed and saved to 'cleaned_file.csv'.")
