 use std::error::Error;
     use std::io;
     use csv::{Reader, Position};
    
     fn main() { example().unwrap(); }
fn example() -> Result<(), Box<dyn Error>> {
         let data = "\
     city,country,popcount
     Boston,United States,4628910
     Concord,United States,42695
     ";
         let rdr = Reader::from_reader(io::Cursor::new(data));
         let mut iter = rdr.into_records();
         let mut pos = Position::new();
         loop {
             // Read the position immediately before each record.
             let next_pos = iter.reader().position().clone();
             if iter.next().is_none() {
                 break;
             }
             pos = next_pos;
         }

         println!("{:?}", pos);
    
         // Now seek the reader back to `pos`. This will let us read the
         // last record again.
         iter.reader_mut().seek(pos)?;
         let mut iter = iter.into_reader().into_records();
         if let Some(result) = iter.next() {
             let record = result?;
             assert_eq!(record, vec!["Concord", "United States", "42695"]);
             Ok(())
         } else {
             Err(From::from("expected at least one record but got none"))
         }
     }
     ```
